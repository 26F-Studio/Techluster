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
        size_t port,
        int db,
        uint32_t timeout,
        int32_t retries,
        uint32_t interval
) {
    _redisClient.connect(
            host,
            port,
            [=, this](const string &host, size_t port, client::connect_state status) {
                if (status == client::connect_state::dropped) {
                    LOG_ERROR << "client disconnected from " << host << ":" << port;
                    connect(host, port, db, timeout, retries, interval);
                } else if (status == client::connect_state::ok) {
                    _redisClient.select(db);
                    _redisClient.sync_commit();
                }
            },
            timeout,
            retries,
            interval
    );
    LOG_INFO << _baseKey << " redis connected.";
}

void RedisHelper::disconnect() {
    if (_redisClient.is_connected()) {
        _redisClient.disconnect();
        LOG_INFO << _baseKey << " redis disconnected.";
    }
}

bool RedisHelper::tokenBucket(
        const string &key,
        const chrono::microseconds &restoreInterval,
        uint64_t maxCount
) {
    const auto countKey = _baseKey + ":tokenBucket:count:" + key;
    const auto updatedKey = _baseKey + ":tokenBucket:updated:" + key;
    auto maxTtl = chrono::duration_cast<chrono::seconds>(restoreInterval * maxCount);

    uint64_t countValue;
    try {
        auto bucketCount = get(countKey);
        countValue = stoull(bucketCount);
    } catch (...) {
        _redisClient.set(countKey, to_string(maxCount - 1));
        countValue = maxCount;
    }

    bool hasToken = true;
    try {
        auto lastUpdated = get(updatedKey);
        auto nowMicroseconds = datetime::toDate().microSecondsSinceEpoch();
        auto generatedCount =
                (nowMicroseconds -
                 datetime::toDate(lastUpdated).microSecondsSinceEpoch()
                ) / restoreInterval.count() - 1;

        if (generatedCount >= 1) {
            _redisClient.set(updatedKey, datetime::toString(nowMicroseconds));
            _redisClient.incrby(countKey, static_cast<int>(generatedCount) - 1);
            hasToken = true;
        } else if (countValue > 0) {
            _redisClient.decr(countKey);
            hasToken = true;
        } else {
            hasToken = false;
        }
    } catch (...) {
        _redisClient.set(updatedKey, datetime::toString());
        _redisClient.set(countKey, to_string(maxCount - 1));
    }

    // Use sync methods to make sure the operation is completed.
    expire({{countKey,   maxTtl},
            {updatedKey, maxTtl}});
    return hasToken;
}

void RedisHelper::del(const string &key) {
    const auto tempKey = _baseKey + ":" + key;
    _redisClient.del({tempKey});
    _redisClient.sync_commit();
}

void RedisHelper::expire(const string &key, const chrono::seconds &ttl) {
    const auto tempKey = _baseKey + ":" + key;
    auto future = _redisClient.expire(tempKey, static_cast<int>(ttl.count()));
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound(tempKey);
    }
}

void RedisHelper::expire(const vector<tuple<string, chrono::seconds>> &params) {
    vector<future<reply>> futures;
    for (const auto &[key, ttl]: params) {
        const auto tempKey = _baseKey + ":" + key;
        futures.push_back(_redisClient.expire(tempKey, static_cast<int>(ttl.count())));
    }
    _redisClient.sync_commit();
    for (auto index = 0; index < futures.size(); ++index) {
        auto reply = futures[index].get();
        if (reply.is_null()) {
            const auto &[key, _] = params[index];
            throw redis_exception::KeyNotFound(key);
        }
    }
}

string RedisHelper::get(const string &key) {
    const auto tempKey = _baseKey + ":" + key;
    auto future = _redisClient.get(tempKey);
    _redisClient.sync_commit();
    auto reply = future.get();
    if (reply.is_null()) {
        throw redis_exception::KeyNotFound(tempKey);
    }
    return reply.as_string();
}

void RedisHelper::setEx(
        const string &key,
        int ttl,
        const string &value
) {
    _redisClient.setex(_baseKey + ":" + key, ttl, value);
    _redisClient.sync_commit();
}

void RedisHelper::setEx(const vector<tuple<string, int, string>> &params) {
    for (const auto &[key, ttl, value]: params) {
        _redisClient.setex(_baseKey + ":" + key, ttl, value);
    }
    _redisClient.sync_commit();
}