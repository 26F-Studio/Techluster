//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <list>
#include <structures/NodeServer.h>
#include <shared_mutex>

namespace tech::plugins {
    class NodeManager : public drogon::Plugin<NodeManager> {
    public:
        using NetEndian = uint64_t;

        NodeManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        structures::NodeServer::Type toType(const std::string &typeString) const;

        void updateNode(structures::NodeServer &&nodeServer);

        std::string getBestNode(const structures::NodeServer::Type &type) const;

        Json::Value parseInfo() const;

        Json::Value parseInfo(const structures::NodeServer::Type &nodeType) const;

    private:
        mutable std::shared_mutex _sharedMutex;

        std::atomic<uint32_t> _waitTimes;
        std::unordered_map<std::string, structures::NodeServer::Type> _typeMapper;
        std::unordered_map<
                structures::NodeServer::Type,
                std::unordered_map<NetEndian, structures::NodeServer>
        > _allNodes;

        void _updateTimer(structures::NodeServer &nodeServer);
    };
}
