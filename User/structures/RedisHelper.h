//
// Created by particleg on 2021/9/29.
//

#pragma once

#include <drogon/drogon.h>
#include <structures/RedisToken.h>
#include <sw/redis++/redis++.h>

namespace tech::structures {
    class RedisHelper : public trantor::NonCopyable {
    private:
        struct Expiration {
            int64_t refresh, access, email;
        };
    public:
        explicit RedisHelper(
                const sw::redis::ConnectionOptions &options,
                Expiration expiration
        );

        explicit RedisHelper(
                const sw::redis::ConnectionOptions &options,
                sw::redis::ConnectionPoolOptions poolOptions,
                Expiration expiration
        );

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

        sw::redis::Redis _redisClient;

        void _compare(
                const std::string &key,
                const std::string &value
        );

        void _expire(
                const std::string &key,
                const std::chrono::duration<uint64_t> &ttl
        );

        std::string _get(const std::string &key);

        void _extendRefreshToken(const std::string &refreshToken);

        std::string _generateRefreshToken(const std::string &userId);

        std::string _generateAccessToken(const std::string &userId);
    };
}
