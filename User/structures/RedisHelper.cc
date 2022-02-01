//
// Created by particleg on 2021/9/29.
//

#include <structures/Exceptions.h>
#include <structures/RedisHelper.h>
#include <utils/crypto.h>
#include <utils/datetime.h>

using namespace drogon;
using namespace std;
using namespace cpp_redis;
using namespace tech::structures;
using namespace tech::utils;

RedisHelper::RedisHelper(Expiration expiration) : _expiration(expiration) {}

RedisHelper::RedisHelper(RedisHelper &&helper) noexcept: _expiration(helper._expiration) {}

void RedisHelper::connect(
        const string &host,
        const size_t &port,
        const uint32_t &timeout,
        const int32_t &retries,
        const uint32_t &interval
) {
    _redisClient.connect(
            host,
            port,
            [](const string &host, size_t port, client::connect_state status) {
                if (status == client::connect_state::dropped) {
                    LOG_ERROR << "client disconnected from " << host << ":" << port;
                }
            },
            timeout,
            retries,
            interval
    );
    LOG_INFO << "Redis connected.";
}

void RedisHelper::disconnect() {
    if (_redisClient.is_connected()) {
        _redisClient.disconnect();
        LOG_INFO << "Redis disconnected.";
    }
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

RedisToken RedisHelper::generateTokens(const string &userId) {
    return {
            _generateRefreshToken(userId),
            _generateAccessToken(userId)
    };
}

void RedisHelper::checkAccessToken(const string &accessToken) {
    _get("player:access:" + accessToken);
}

void RedisHelper::checkEmailCode(
        const string &email,
        const string &code
) {
    _compare("player:code:email:" + email, code);
}

void RedisHelper::deleteEmailCode(const string &email) {
    _redisClient.del({"player:code:email:" + email});
    _redisClient.sync_commit();
}

void RedisHelper::setEmailCode(
        const string &email,
        const string &code
) {
    _redisClient.setex(
            "player:code:email:" + email,
            _expiration.getEmailSeconds(),
            code
    );
    _redisClient.sync_commit();
}

int64_t RedisHelper::getIdByAccessToken(const string &accessToken) {
    return stoll(_get("player:access:" + accessToken));
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
            _redisClient.incrby("tokenBucketCount:" + key, static_cast<int>(generatedCount) - 1);
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
        const chrono::seconds &ttl
) {
    auto future = _redisClient.expire(key, static_cast<int>(ttl.count()));
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
}

string RedisHelper::_get(const string &key) {
    auto future = _redisClient.get(key);
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
    return reply.as_string();
}

void RedisHelper::_setEx(
        const string &key,
        const int &ttl,
        const string &value
) {
    auto future = _redisClient.setex(key, ttl, value);
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
}

void RedisHelper::_extendRefreshToken(const string &refreshToken) {
    _expire(
            "player:refresh:" + refreshToken,
            _expiration.refresh
    );
}

string RedisHelper::_generateRefreshToken(const string &userId) {
    auto refreshToken = crypto::keccak(drogon::utils::getUuid());
    _setEx(
            "player:refresh:" + refreshToken,
            _expiration.getRefreshSeconds(),
            userId
    );
    return refreshToken;
}

string RedisHelper::_generateAccessToken(const string &userId) {
    auto accessToken = crypto::blake2B(drogon::utils::getUuid());
    _setEx(
            "player:id:" + userId,
            _expiration.getAccessSeconds(),
            accessToken
    );
    _setEx(
            "player:access:" + accessToken,
            _expiration.getAccessSeconds(),
            userId
    );
    return accessToken;
}
