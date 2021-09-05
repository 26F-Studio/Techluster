//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <unordered_set>

namespace tech::plugins {
    class Configurator : public drogon::Plugin<Configurator> {
    public:
        Configurator() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] bool isSuperusers(const int64_t &uid) const;

        [[nodiscard]] bool checkCredential(const std::string &credential) const;

    private:
        std::string _credential{}, _connectGateway{};
        Json::Value _heartbeatBody{};
        std::unordered_set<int64_t> _superusers{};
    };
}

