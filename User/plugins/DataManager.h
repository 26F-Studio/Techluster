//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <structures/RedisHelper.h>
#include <drogon/plugins/Plugin.h>
#include <models/Player.h>

namespace tech::plugins {
    class DataManager : public drogon::Plugin<DataManager> {
    public:
        DataManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        tech::structures::RedisToken refresh(
                const std::string &refreshToken
        );

        std::string verifyEmail(const std::string &email);

        tech::structures::RedisToken loginEmailCode(
                const std::string &email,
                const std::string &code
        );

        tech::structures::RedisToken loginEmailPassword(
                const std::string &email,
                const std::string &password
        );

        void resetEmail(
                const std::string &email,
                const std::string &code,
                const std::string &newPassword
        );

        void migrateEmail(
                const std::string &accessToken,
                const std::string &newEmail,
                const std::string &code
        );

        Json::Value getUserInfo(
                const std::string &accessToken,
                const int32_t &userId
        );

        void updateUserInfo(
                const std::string &accessToken,
                const Json::Value &info
        );

        std::string getUserAvatar(
                const std::string &accessToken,
                const int32_t &userId
        );

        [[nodiscard]] bool ipLimit(const std::string &ip) const;

        [[nodiscard]] bool emailLimit(const std::string &email);

    private:
        std::chrono::seconds _ipInterval{}, _emailInterval{};
        uint64_t _ipMaxCount{}, _emailMaxCount{};

        drogon::orm::DbClientPtr _pgClient;
        std::unique_ptr<tech::structures::RedisHelper> _redisHelper;
        std::unique_ptr<drogon::orm::Mapper<
                drogon_model::Techluster::Player
        >> _playerMapper;
    };
}

