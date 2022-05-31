//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <plugins/BaseTester.h>
#include <types/Permission.h>

namespace tech::plugins {
    class GamingTester : public BaseTester<GamingTester> {
    public:
        struct Player {
            const int64_t id;
            const std::string username;
            const std::string motto;
            const types::Permission permission;
            const std::string email;
            const std::string accessToken;
            const std::string refreshToken;
        };

        GamingTester() = default;

        void processConfig(const Json::Value &config) override;

        void connect();

        void disconnect();

    private:
        std::string _userNode;
        std::string _password;
        std::vector<std::string> _emails;
        std::unordered_map<int64_t, drogon::WebSocketClientPtr> _wsMap;
    };
}

