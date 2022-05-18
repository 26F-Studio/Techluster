//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/drogon.h>
#include <shared_mutex>
#include <types/NodeType.h>

namespace tech::structures {
    class NodeServer {
    public:
        NodeServer(
                types::NodeType type,
                const std::string &ip,
                uint16_t port,
                double taskInterval,
                std::string description,
                Json::Value info
        );

        NodeServer(NodeServer &&nodeServer) noexcept;

        Json::Value getInfo() const;

        void setInfo(Json::Value info);

        int64_t getLastSeen() const;

        void setLastSeen();

        Json::Value parseNode() const;

        const trantor::InetAddress address;
        const types::NodeType nodeType;
        const std::string description;
        const double taskInterval;
        std::atomic<uint64_t> removalTimerId;
    private:
        mutable std::shared_mutex _sharedMutex;
        trantor::Date _lastSeen;
        Json::Value _info;
    };
}
