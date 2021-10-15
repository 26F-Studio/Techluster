//
// Created by particleg on 2021/9/29.
//

#include <structures/Exceptions.h>
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

RedisToken RedisHelper::refresh(const string &refreshToken) {
    _extendRefreshToken(refreshToken);
    return {
            refreshToken,
            _generateAccessToken(
                    _get("player:refresh:" + refreshToken)
            )
    };
}

RedisToken RedisHelper::generateTokens(const string &userId) {
    return {
            _generateRefreshToken(userId),
            _generateAccessToken(userId)
    };
}

void RedisHelper::checkAccessToken(const string &accessToken) {
    _get("player:id:" + accessToken);
}

void RedisHelper::checkEmailCode(
        const string &email,
        const string &code
) {
    _compare("player:code:email_" + email, code);
}

void RedisHelper::deleteEmailCode(const string &email) {
    _redisClient.del("player:code:email_" + email);
}

void RedisHelper::setEmailCode(
        const string &email,
        const string &code
) {
    _redisClient.set(
            "player:code:email_" + email,
            code,
            chrono::minutes(_expiration.email)
    );
}

int64_t RedisHelper::getUserId(const string &accessToken) {
    return stoll(_get("player:id:" + accessToken));
}

bool RedisHelper::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        const uint64_t &maxCount
) {
    auto maxTtl = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

    auto setCount = [this, &key](const uint64_t &count) {
        _redisClient.set(
                "tokenBucketCount:" + key,
                to_string(count)
        );
    };

    auto setDate = [this, &key](const string &dateStr) {
        _redisClient.set(
                "tokenBucketUpdated:" + key,
                dateStr
        );
    };

    auto checkCount = [this, &key](const uint64_t &count) -> bool {
        if (count > 0) {
            _redisClient.decr("tokenBucketCount:" + key);
            return true;
        }
        return false;
    };

    uint64_t countValue;
    try {
        auto bucketCount = _get("tokenBucketCount:" + key);
        countValue = stoull(bucketCount);
    } catch (...) {
        setCount(maxCount - 1);
        countValue = maxCount;
    }

    bool hasToken = true;
    try {
        auto lastUpdated = _get("tokenBucketUpdated:" + key);
        auto nowMicroseconds = datetime::toDate().microSecondsSinceEpoch();
        auto generatedCount =
                (nowMicroseconds -
                 datetime::toDate(lastUpdated).microSecondsSinceEpoch()
                ) / restoreInterval.count() - 1;

        if (generatedCount >= 1) {
            setDate(datetime::toString(nowMicroseconds));
            _redisClient.incrby("tokenBucketCount:" + key, generatedCount - 1);
            hasToken = true;
        } else {
            hasToken = checkCount(countValue);
        }
    } catch (...) {
        setDate(datetime::toString());
        setCount(maxCount - 1);
    }

    _expire("tokenBucketCount:" + key, maxTtl);
    _expire("tokenBucketUpdated:" + key, maxTtl);
    return hasToken;
}

void RedisHelper::_compare(
        const string &key,
        const string &value
) {
    if (_get(key) != value) {
        throw redis_exception::NotEqual("key = " + key);
    }
}

void RedisHelper::_expire(
        const string &key,
        const chrono::duration <uint64_t> &ttl
) {
    if (!_redisClient.expire(key, ttl)) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
}

string RedisHelper::_get(const string &key) {
    auto result = _redisClient.get(key);
    if (!result) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
    return result.value();
}

void RedisHelper::_extendRefreshToken(const string &refreshToken) {
    _expire(
            "player:refresh:" + refreshToken,
            chrono::minutes(_expiration.refresh)
    );
}

string RedisHelper::_generateRefreshToken(const string &userId) {
    auto refreshToken = crypto::keccak(drogon::utils::getUuid());
    _redisClient.set(
            "player:refresh:" + refreshToken,
            userId,
            chrono::minutes(_expiration.refresh)
    );
    return refreshToken;
}

string RedisHelper::_generateAccessToken(const string &userId) {
    auto accessToken = crypto::blake2B(drogon::utils::getUuid());
    _redisClient.set(
            "player:access:" + userId,
            accessToken,
            chrono::minutes(_expiration.access)
    );
    _redisClient.set(
            "player:id:" + accessToken,
            userId,
            chrono::minutes(_expiration.access)
    );
    return accessToken;
}
