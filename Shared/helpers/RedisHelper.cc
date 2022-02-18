//
// Created by ParticleG on 2022/2/9.
//

#include <structures/Exceptions.h>
#include <helpers/RedisHelper.h>
#include <utils/crypto.h>
#include <utils/datetime.h>

using namespace cpp_redis;
using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::utils;

RedisHelper::RedisHelper(std::string BaseKey) : _baseKey(std::move(BaseKey)) {}

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
            [this, timeout, retries, interval](const string &host, size_t port, client::connect_state status) {
                if (status == client::connect_state::dropped) {
                    LOG_ERROR << "client disconnected from " << host << ":" << port;
                    connect(host, port, timeout, retries, interval);
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

// Make this method safe and efficient.
bool RedisHelper::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        const uint64_t &maxCount
) {
    auto maxTtl = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

    auto setCount = [this, &key](const uint64_t &count) {
        _redisClient.set(
                _baseKey + ":tokenBucket:count:" + key,
                to_string(count)
        );
    };

    auto setDate = [this, &key](const string &dateStr) {
        _redisClient.set(
                _baseKey + ":tokenBucket:updated:" + key,
                dateStr
        );
    };

    auto checkCount = [this, &key](const uint64_t &count) -> bool {
        if (count > 0) {
            _redisClient.decr(_baseKey + ":tokenBucket:count:" + key);
            return true;
        }
        return false;
    };

    uint64_t countValue;
    try {
        auto bucketCount = get(_baseKey + ":tokenBucket:count:" + key);
        countValue = stoull(bucketCount);
    } catch (...) {
        setCount(maxCount - 1);
        countValue = maxCount;
    }

    bool hasToken = true;
    try {
        auto lastUpdated = get(_baseKey + ":tokenBucket:updated:" + key);
        auto nowMicroseconds = datetime::toDate().microSecondsSinceEpoch();
        auto generatedCount =
                (nowMicroseconds -
                 datetime::toDate(lastUpdated).microSecondsSinceEpoch()
                ) / restoreInterval.count() - 1;

        if (generatedCount >= 1) {
            setDate(datetime::toString(nowMicroseconds));
            _redisClient.incrby(_baseKey + ":tokenBucket:count:" + key, static_cast<int>(generatedCount) - 1);
            hasToken = true;
        } else {
            hasToken = checkCount(countValue);
        }
    } catch (...) {
        setDate(datetime::toString());
        setCount(maxCount - 1);
    }

    expire(_baseKey + ":tokenBucket:count:" + key, maxTtl);
    expire(_baseKey + ":tokenBucket:updated:" + key, maxTtl);
    return hasToken;
}

void RedisHelper::compare(
        const string &key,
        const string &value
) {
    if (get(key) != value) {
        throw redis_exception::NotEqual("key = " + key);
    }
}

void RedisHelper::del(const string &key) {
    _redisClient.del({key});
    _redisClient.sync_commit();
}

void RedisHelper::expire(
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

string RedisHelper::get(const string &key) {
    auto future = _redisClient.get(key);
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound("Key = " + key);
    }
    return reply.as_string();
}

void RedisHelper::setEx(
        const string &key,
        const int &ttl,
        const string &value
) {
    _redisClient.setex(key, ttl, value);
    _redisClient.sync_commit();
}