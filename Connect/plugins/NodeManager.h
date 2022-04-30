//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/I18nHelper.h>
#include <structures/NodeServer.h>
#include <shared_mutex>

namespace tech::plugins {
    class NodeManager :
            public drogon::Plugin<NodeManager>,
            public helpers::I18nHelper<NodeManager> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void updateNode(structures::NodeServer &&nodeServer);

        std::string getBestNode(const types::NodeType &nodeType) const;

        Json::Value getAllNodes(const types::NodeType &nodeType) const;

        void checkNode(types::NodeType nodeType, trantor::InetAddress address) const;

        Json::Value parseInfo() const;

        Json::Value parseInfo(const types::NodeType &nodeType) const;

    private:
        mutable std::shared_mutex _sharedMutex;

        std::atomic<uint32_t> _waitTimes;
        std::unordered_map<
                types::NodeType,
                std::unordered_map<uint64_t, structures::NodeServer>
        > _nodeMap;

        void _updateTimer(structures::NodeServer &nodeServer);
    };
}
