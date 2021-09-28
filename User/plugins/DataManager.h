//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <sw/redis++/redis++.h>
#include <drogon/plugins/Plugin.h>
#include <models/Player.h>

namespace tech::plugins {
    class DataManager : public drogon::Plugin<DataManager> {
    public:
        struct Token {
            std::string refresh;
            std::string access;
        };

        DataManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        Token refresh(const std::string &refreshToken);

        std::string verifyEmail(const std::string &email);

        Token loginEmailCode(
                const std::string &email,
                const std::string &code
        );

        Token loginEmailPassword(
                const std::string &email,
                const std::string &password
        );

        Token loginWeChat(const std::string &code);

        [[nodiscard]] bool ipLimit(const std::string &ip) const {
            return tokenBucket(
                    "ip:" + ip,
                    _ipInterval,
                    _ipMaxCount
            );
        }

        [[nodiscard]] bool emailLimit(const std::string &email) {
            return tokenBucket(
                    "email:" + email,
                    _emailInterval,
                    _emailMaxCount
            );
        }

        bool coolDown(const std::string &key, const std::chrono::seconds &interval);

    private:
        std::chrono::seconds _ipInterval{}, _emailInterval{};
        uint64_t _ipMaxCount{}, _emailMaxCount{};

        int64_t refreshExpiration{}, accessExpiration{}, emailExpiration{};

        drogon::orm::DbClientPtr _pgClient;
        std::unique_ptr<sw::redis::Redis> _redisClient;
        std::unique_ptr<drogon::orm::Mapper<drogon_model::Techluster::Player>> _playerMapper;

        std::string _generateRefreshToken(const std::string &userId);

        std::string _generateAccessToken(const std::string &userId);

        Token generateTokens(const std::string &userId);

        [[nodiscard]] bool tokenBucket(
                const std::string &key,
                const std::chrono::microseconds &restoreInterval,
                const uint64_t &maxCount
        ) const;
    };
}

