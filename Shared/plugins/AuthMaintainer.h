//
// Created by Particle_G on 2021/9/10.
//

#pragma once

#include <drogon/plugins/Plugin.h>

namespace tech::plugins {
    class AuthMaintainer : public drogon::Plugin<AuthMaintainer> {
    public:
        AuthMaintainer() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        trantor::InetAddress getAuthAddress() const;

    private:
        std::chrono::duration<double> _taskMinutes{};
        std::atomic<bool> _authNodeDown{};
        std::atomic<trantor::InetAddress> _connectAddress{}, _authAddress{};

        void updateAuthAddress();
    };
}