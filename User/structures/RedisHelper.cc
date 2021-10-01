//
// Created by particleg on 2021/9/29.
//

#include <structures/RedisHelper.h>
#include <utils/crypto.h>
#include <utils/datetime.h>

using namespace drogon;
using namespace std;
using namespace sw::redis;
using namespace tech::structures;
using namespace tech::utils;

RedisHelper::RedisHelper(
        const ConnectionOptions &options,
        Expiration expiration
) : _redisClient(options),
    _expiration(expiration) {}

RedisHelper::RedisHelper(
        const ConnectionOptions &options,
        ConnectionPoolOptions poolOptions,
        Expiration expiration
) : _redisClient(options, poolOptions),
    _expiration(expiration) {}

RedisToken RedisHelper::generateTokens(const string &userId) {
    return {
            _generateRefreshToken(userId),
            _generateAccessToken(userId)
    };
}

RedisToken RedisHelper::refresh(const string &refreshToken) {
    _extendRefreshToken(refreshToken);
    return {
            refreshToken,
            _generateAccessToken(
                    _get("player:refresh:" + refreshToken)
            )
    };
}

void RedisHelper::checkAccessToken(const string &accessToken) {
    if (!_redisClient.get("player:id:" + accessToken).has_value()) {
        throw range_error("Invalid accessToken");
    }
}

void RedisHelper::checkEmailCode(
        const string &email,
        const string &code
) {
    if (_get("player:code:email_" + email) != code) {
        throw range_error("Invalid code");
    }
}

void RedisHelper::deleteEmailCode(const string &email) {
    _del("player:code:email_" + email);
}

void RedisHelper::setEmailCode(
        const string &email,
        const string &code
) {
    if (!_redisClient.set(
            "player:code:email_" + email,
            code,
            chrono::minutes(_expiration.email)
    )) {
        throw range_error("Set player:code:email_" + email + " Failed");
    }
}

int64_t RedisHelper::getUserId(const string &accessToken) {
    return stoll(_get("player:id:" + accessToken));
}

bool RedisHelper::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        const uint64_t &maxCount
) {
    auto maxTTL = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

    auto setCount = [this, &key](const uint64_t &count) {
        if (!_redisClient.set(
                "tokenBucketCount:" + key,
                to_string(count)
        )) {
            throw range_error("Set tokenBucketCount:" + key + " Failed");
        }
    };

    auto setDate = [this, &key](const string &dateStr) {
        if (!_redisClient.set(
                "tokenBucketUpdated:" + key,
                dateStr
        )) {
            throw range_error("Set tokenBucketUpdated:" + key + " Failed");
        }
    };

    auto checkCount = [this, &key](const uint64_t &count) -> bool {
        if (count > 0) {
            _redisClient.decr("tokenBucketCount:" + key);
            return true;
        }
        return false;
    };

    auto bucketCount = _redisClient.get("tokenBucketCount:" + key);

    uint64_t countValue;

    if (!bucketCount) {
        setCount(maxCount - 1);
        countValue = maxCount;
    } else {
        countValue = stoull(bucketCount.value());
    }

    bool hasToken = true;
    auto lastUpdated = _redisClient.get("tokenBucketUpdated:" + key);
    if (!lastUpdated) {
        setDate(datetime::toString());
        setCount(maxCount - 1);
    } else {
        auto nowMicroseconds = datetime::toDate().microSecondsSinceEpoch();
        auto generatedCount =
                (nowMicroseconds -
                 datetime::toDate(lastUpdated.value()).microSecondsSinceEpoch()
                ) / restoreInterval.count() - 1;

        if (generatedCount >= 1) {
            setDate(datetime::toString(nowMicroseconds));
            _redisClient.incrby("tokenBucketCount:" + key, generatedCount - 1);
            hasToken = true;
        } else {
            hasToken = checkCount(countValue);
        }
    }
    _redisClient.expire("tokenBucketCount:" + key, maxTTL);
    _redisClient.expire("tokenBucketUpdated:" + key, maxTTL);
    return hasToken;
}

void RedisHelper::_del(const string &key) {
    if (!_redisClient.del(key)) {
        throw out_of_range("key = " + key + " not found");
    }
}

void RedisHelper::_expire(
        const string &key,
        const chrono::duration<uint64_t> &ttl
) {
    if (!_redisClient.expire(key, ttl)) {
        throw out_of_range("key = " + key + " not found");
    }
}

string RedisHelper::_get(const string &key) {
    auto result = _redisClient.get(key);
    if (!result) {
        throw out_of_range("key = " + key + " not found");
    }
    return result.value();
}

void RedisHelper::_set(
        const string &key,
        const string &value,
        const chrono::milliseconds &ttl,
        const UpdateType &updateType
) {
    if (!_redisClient.set(key, value, ttl, updateType)) {
        throw range_error("Set key = " + key + " Failed");
    }
}

void RedisHelper::_extendRefreshToken(const string &refreshToken) {
    _expire(
            "player:refresh:" + refreshToken,
            chrono::minutes(_expiration.refresh)
    );
}

string RedisHelper::_generateRefreshToken(const string &userId) {
    auto refreshToken = crypto::keccak(drogon::utils::getUuid());
    _set(
            "player:refresh:" + refreshToken,
            userId,
            chrono::minutes(_expiration.refresh)
    );
    return refreshToken;
}

string RedisHelper::_generateAccessToken(const string &userId) {
    auto accessToken = crypto::blake2b(drogon::utils::getUuid());
    _set(
            "player:access:" + userId,
            accessToken,
            chrono::minutes(_expiration.access)
    );
    _set(
            "player:id:" + accessToken,
            userId,
            chrono::minutes(_expiration.access)
    );
    return accessToken;
}
