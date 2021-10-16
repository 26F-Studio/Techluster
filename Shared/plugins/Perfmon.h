//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <shared_mutex>
#include <unordered_set>

namespace tech::plugins {
    class Perfmon : public drogon::Plugin<Perfmon> {
    public:
        Perfmon() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] Json::Value parseInfo() const;

    private:
        std::string _reportAddress{};
        Json::Value _heartbeatBody{};
        std::atomic<uint32_t> _cpuInterval{};
        std::atomic<uint64_t> _vMemTotal{}, _vMemAvail{}, _rMemTotal{}, _rMemAvail{},
                _netConn{}, _diskUser{}, _diskAvail{}, _diskTotal{};
        std::atomic<int64_t> _netDown{}, _netUp{};
        std::atomic<double> _cpuLoad{}, _taskInterval{};

        void _report();

        void _updateInfo();
    };
}

