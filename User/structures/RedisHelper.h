//
// Created by particleg on 2021/9/29.
//

#pragma once

#include <drogon/drogon.h>
#include <structures/RedisToken.h>
#include <cpp_redis/cpp_redis>

namespace tech::structures {
    class RedisHelper : public trantor::NonCopyable {
    private:
        struct Expiration {
            [[nodiscard]] int getRefreshSeconds() const {
                return static_cast<int>(refresh.count());
            }

            [[nodiscard]] int getAccessSeconds() const {
                return static_cast<int>(access.count());
            }

            [[nodiscard]] int getEmailSeconds() const {
                return static_cast<int>(email.count());
            }

            std::chrono::seconds refresh{}, access{}, email{};
        };

    public:
        explicit RedisHelper(Expiration expiration);

        RedisHelper(RedisHelper &&helper)

        noexcept;

        void connect(
                const std::string &host = "127.0.0.1",
                const size_t &port = 6379,
                const uint32_t &timeout = 0,
                const int32_t &retries = 0,
                const uint32_t &interval = 0
        );

        void disconnect();

        RedisToken refresh(const std::string &refreshToken);

        [[nodiscard]] RedisToken generateTokens(const std::string &userId);

        void checkAccessToken(const std::string &accessToken);

        void checkEmailCode(
                const std::string &email,
                const std::string &code
        );

        void deleteEmailCode(const std::string &email);

        void setEmailCode(
                const std::string &email,
                const std::string &code
        );

        [[nodiscard]] int64_t getUserId(const std::string &accessToken);

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                const uint64_t &maxCount
        );


    private:
        const Expiration _expiration;

        cpp_redis::client _redisClient;

        void _compare(
                const std::string &key,
                const std::string &value
        );

        void _expire(
                const std::string &key,
                const std::chrono::seconds &ttl
        );

        std::string _get(const std::string &key);

        void _setEx(
                const std::string &key,
                const int &ttl,
                const std::string &value
        );

        // TODO: Add more wrappers for basic redis commands

        void _extendRefreshToken(const std::string &refreshToken);

        std::string _generateRefreshToken(const std::string &userId);

        std::string _generateAccessToken(const std::string &userId);
    };
}
