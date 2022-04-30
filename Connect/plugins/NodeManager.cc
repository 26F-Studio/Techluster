//
// Created by Particle_G on 2021/8/19.
//

#include <drogon/drogon.h>
#include <magic_enum.hpp>
#include <structures/Exceptions.h>
#include <types/NetEndian.h>
#include <plugins/NodeManager.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void NodeManager::initAndStart(const Json::Value &config) {
    if (!config["heartBeat"]["waitTimes"].isUInt()) {
        LOG_ERROR << R"(Invalid heartBeat config)";
        abort();
    } else {
        _waitTimes = config["heartBeat"]["waitTimes"].asUInt();
    }

    LOG_INFO << "NodeManager loaded.";
}

void NodeManager::shutdown() { LOG_INFO << "NodeManager shutdown."; }

void NodeManager::updateNode(NodeServer &&nodeServer) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    auto [nodePair, isNew] = _nodeMap[nodeServer.nodeType].try_emplace(
            NetEndian(nodeServer.address).netEndian,
            move(nodeServer)
    );
    if (!isNew) {
        nodePair->second.setInfo(move(nodeServer.getInfo()));
    }
    nodePair->second.setLastSeen();
    _updateTimer(nodePair->second);
}

Json::Value NodeManager::getAllNodes(const NodeType &nodeType) const {
    Json::Value result(Json::arrayValue);
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (const auto &[_, nodeServer]: _nodeMap.at(nodeType)) {
            result.append(nodeServer.address.toIpPort());
        }
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notAvailable."s.append(enum_name(nodeType))),
                ResultCode::notAvailable,
                k503ServiceUnavailable
        );
    }
    return result;
}

string NodeManager::getBestNode(const NodeType &nodeType) const {
    //TODO: implement logics
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        return _nodeMap.at(nodeType).begin()->second.address.toIpPort();
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notAvailable."s.append(enum_name(nodeType))),
                ResultCode::notAvailable,
                k503ServiceUnavailable
        );
    }
}

void NodeManager::checkNode(NodeType nodeType, trantor::InetAddress address) const {
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        if (!_nodeMap.at(nodeType).contains(NetEndian(address).netEndian)) {
            throw ResponseException(
                    i18n("notFound."s.append(enum_name(nodeType))),
                    ResultCode::notFound,
                    k404NotFound
            );
        }
    } catch (const out_of_range &) {
        throw ResponseException(
                i18n("notFound."s.append(enum_name(nodeType))),
                ResultCode::notFound,
                k404NotFound
        );
    }
}

Json::Value NodeManager::parseInfo() const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &[type, nodeList]: _nodeMap) {
        Json::Value tempType;
        tempType["typeId"] = static_cast<int>(type);
        tempType["nodeList"] = Json::arrayValue;
        for (const auto &[_, node]: nodeList) {
            tempType["nodeList"].append(node.parseNode());
        }
        result.append(tempType);
    }
    return result;
}

Json::Value NodeManager::parseInfo(const NodeType &nodeType) const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &[type, nodeList]: _nodeMap) {
        if (type == nodeType) {
            for (const auto &[_, node]: nodeList) {
                result.append(node.parseNode());
            }
            break;
        }
    }
    return result;
}

void NodeManager::_updateTimer(NodeServer &nodeServer) {
    if (_waitTimes == 0) {
        // Disable heartbeat timeout
        return;
    }
    // Invalidate old heartbeat timeout
    app().getLoop()->invalidateTimer(nodeServer.removalTimerId);

    const auto &nodeType = nodeServer.nodeType;
    const auto &netEndian = NetEndian(nodeServer.address).netEndian;
    // Set new heartbeat timeout
    nodeServer.removalTimerId = app().getLoop()->runAfter(
            nodeServer.taskInterval * _waitTimes,
            [this, nodeType, netEndian]() {
                unordered_map<uint64_t, NodeServer>::node_type node; // Declare node first to avoid memory leak
                unique_lock<shared_mutex> lock(_sharedMutex);
                node = _nodeMap[nodeType].extract(netEndian);
                if (node.empty()) {
                    LOG_WARN << "Node " << netEndian << " already removed";
                }
            }
    );
}
