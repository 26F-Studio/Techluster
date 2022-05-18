//
// Created by Particle_G on 2021/9/10.
//

#pragma once

#include <drogon/HttpController.h>
#include <drogon/plugins/Plugin.h>
#include <types/NodeType.h>

namespace tech::plugins {
    class NodeMaintainer : public drogon::Plugin<NodeMaintainer> {
    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        bool checkNode(types::NodeType type, const std::string &address) const;

        drogon::HttpStatusCode checkAccessToken(const std::string &accessToken, int64_t &id);

        Json::Value getUserInfo(int64_t userId);

        std::tuple<std::string, std::string> getWorkshopItem(const std::string &itemId);

        std::chrono::duration<double> _taskMinutes{};
        std::atomic<trantor::InetAddress> connectAddress{}, userAddress{}, workshopAddress{};
    private:
        void _updateUserAddress();

        void _updateWorkshopAddress();
    };
}