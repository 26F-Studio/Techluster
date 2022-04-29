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
                size_t port = 6379,
                int db = 0,
                uint32_t timeout = 0,
                int32_t retries = 0,
                uint32_t interval = 0
        );

        void disconnect();

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                uint64_t maxCount
        );

        virtual ~RedisHelper() = default;

    protected:
        void del(const std::string &key);

        void expire(const std::string &key, const std::chrono::seconds &ttl);

        void expire(const std::vector<std::tuple<std::string, std::chrono::seconds>> &params);

        std::string get(const std::string &key);

        void setEx(
                const std::string &key,
                int ttl,
                const std::string &value
        );

        void setEx(const std::vector<std::tuple<std::string, int, std::string>> &params);

    private:
        cpp_redis::client _redisClient;
        std::string _baseKey;
    };
}



