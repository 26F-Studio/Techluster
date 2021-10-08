//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/RedisHelper.h>

namespace tech::plugins {
    class RoomManager : public drogon::Plugin<RoomManager> {
    public:
        RoomManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        std::string createRoom(
                const uint32_t &capacity,
                const std::string &password,
                const Json::Value &info,
                const Json::Value &config
        );

        void joinRoom(const std::string &roomId);

    private:
        std::unique_ptr<tech::structures::RedisHelper> _redisHelper;
    };
}

