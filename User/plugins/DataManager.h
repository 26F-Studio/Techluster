//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <models/Data.h>
#include <models/Player.h>
#include <structures/DataField.h>
#include <structures/RedisHelper.h>

namespace tech::plugins {
    class DataManager : public drogon::Plugin<DataManager> {
    public:
        DataManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        inline void checkAccessToken(
                const std::string &accessToken
        );

        inline tech::structures::RedisToken refresh(
                const std::string &refreshToken
        );

        inline std::string verifyEmail(const std::string &email);

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
                const int64_t &userId
        );

        void updateUserInfo(
                const std::string &accessToken,
                const Json::Value &info
        );

        std::string getUserAvatar(
                const std::string &accessToken,
                const int64_t &userId
        );

        Json::Value getUserData(
                const std::string &accessToken,
                const int64_t &userId,
                const tech::structures::DataField &field,
                const Json::Value &list
        );

        void updateUserData(
                const std::string &accessToken,
                const int64_t &userId,
                const tech::structures::DataField &field,
                const Json::Value &list
        );

        [[nodiscard]] inline bool ipLimit(const std::string &ip) const;

        [[nodiscard]] inline bool emailLimit(const std::string &email) const;

    private:
        std::chrono::seconds _ipInterval{}, _emailInterval{};
        uint64_t _ipMaxCount{}, _emailMaxCount{};

        drogon::orm::DbClientPtr _pgClient;
        std::unique_ptr<tech::structures::RedisHelper> _redisHelper;
        std::unique_ptr<drogon::orm::Mapper<
                drogon_model::Techluster::Data
        >> _dataMapper;
        std::unique_ptr<drogon::orm::Mapper<
                drogon_model::Techluster::Player
        >> _playerMapper;
    };
}

