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

        inline void checkAccessToken(const std::string &accessToken);

        inline void checkEmailCode(
                const std::string &email,
                const std::string &code
        );

        void deleteEmailCode(const std::string &email);

        inline void setEmailCode(
                const std::string &email,
                const std::string &code
        );

        [[nodiscard]] inline int64_t getUserId(const std::string &accessToken);

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                const uint64_t &maxCount
        );

    private:
        const Expiration _expiration;

        sw::redis::Redis _redisClient;

        inline void _compare(
                const std::string &key,
                const std::string &value
        );

        inline void _del(const std::string &key);

        inline void _expire(
                const std::string &key,
                const std::chrono::duration<uint64_t> &ttl
        );

        inline std::string _get(const std::string &key);

        inline void _set(
                const std::string &key,
                const std::string &value,
                const std::chrono::milliseconds &ttl = std::chrono::milliseconds(0),
                const sw::redis::UpdateType &updateType = sw::redis::UpdateType::ALWAYS
        );

        inline void _extendRefreshToken(const std::string &refreshToken);

        inline std::string _generateRefreshToken(const std::string &userId);

        inline std::string _generateAccessToken(const std::string &userId);
    };
}
