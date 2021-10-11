//
// Created by particleg on 2021/9/29.
//

#include <structures/Exceptions.h>
#include <structures/RedisHelper.h>
#include <utils/crypto.h>
#include <utils/datetime.h>
#include <utils/serializer.h>

using namespace drogon;
using namespace std;
using namespace sw::redis;
using namespace tech::structures;
using namespace tech::utils;

RedisHelper::RedisHelper(
        const ConnectionOptions &options
) : _redisClient(options) {}

RedisHelper::RedisHelper(
        const ConnectionOptions &options,
        ConnectionPoolOptions poolOptions
) : _redisClient(options, poolOptions) {}

bool RedisHelper::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        const uint64_t &maxCount
) {
    auto maxTTL = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

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

    _expire("tokenBucketCount:" + key, maxTTL);
    _expire("tokenBucketUpdated:" + key, maxTTL);
    return hasToken;
}

inline void RedisHelper::_compare(
        const string &key,
        const string &value
) {
    if (_get(key) != value) {
        throw RedisException::NotEqual("key = " + key);
    }
}

inline void RedisHelper::_expire(
        const string &key,
        const chrono::duration<uint64_t> &ttl
) {
    if (!_redisClient.expire(key, ttl)) {
        throw RedisException::KeyNotFound("Key = " + key);
    }
}

inline string RedisHelper::_get(const string &key) {
    auto result = _redisClient.get(key);
    if (!result) {
        throw RedisException::KeyNotFound("Key = " + key);
    }
    return result.value();
}

inline string RedisHelper::_hget(const string &key, const string &field) {
    auto result = _redisClient.hget(key, field);
    if (!result) {
        throw RedisException::FieldNotFound("Key = " + key + ", Field = " + field);
    }
    return result.value();
}
