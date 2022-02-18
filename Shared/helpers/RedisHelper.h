//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <drogon/drogon.h>
#include <cpp_redis/cpp_redis>

namespace tech::helpers {
    class RedisHelper : public trantor::NonCopyable {
    public:
        explicit RedisHelper(std::string BaseKey = CMAKE_PROJECT_NAME);

        void connect(
                const std::string &host = "127.0.0.1",
                const size_t &port = 6379,
                const uint32_t &timeout = 0,
                const int32_t &retries = 0,
                const uint32_t &interval = 0
        );

        void disconnect();

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                const uint64_t &maxCount
        );

        virtual ~RedisHelper() = default;

    protected:
        void compare(
                const std::string &key,
                const std::string &value
        );

        void del(const std::string &key);

        void expire(
                const std::string &key,
                const std::chrono::seconds &ttl
        );

        std::string get(const std::string &key);

        void setEx(
                const std::string &key,
                const int &ttl,
                const std::string &value
        );

    private:
        cpp_redis::client _redisClient;
        std::string _baseKey;
    };
}



