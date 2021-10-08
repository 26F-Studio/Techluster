//
// Created by particleg on 2021/9/29.
//

#pragma once

#include <drogon/drogon.h>
#include <sw/redis++/redis++.h>

namespace tech::structures {
    class RedisHelper : public trantor::NonCopyable {
    public:
        explicit RedisHelper(
                const sw::redis::ConnectionOptions &options
        );

        explicit RedisHelper(
                const sw::redis::ConnectionOptions &options,
                sw::redis::ConnectionPoolOptions poolOptions
        );

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                const uint64_t &maxCount
        );

        inline std::string createRoom(
                const uint32_t &capacity,
                const std::string &passwordHash,
                const Json::Value &info,
                const Json::Value &config
        );

        inline void joinRoom(
                const std::string &roomId,
                const int64_t &userId
        );

    private:
        sw::redis::Redis _redisClient;

        inline void _compare(
                const std::string &key,
                const std::string &value
        );

        inline void _expire(
                const std::string &key,
                const std::chrono::duration<uint64_t> &ttl
        );

        inline std::string _get(const std::string &key);

        inline std::string _hget(const std::string &key, const std::string &field);

        inline void _checkRoom(const std::string &roomId);
    };
}
