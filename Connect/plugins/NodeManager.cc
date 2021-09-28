//
// Created by Particle_G on 2021/8/19.
//

#include <drogon/drogon.h>
#include <plugins/NodeManager.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::structures;

void NodeManager::initAndStart(const Json::Value &config) {
    if (!(
            config.isMember("nodeType") && config["nodeType"].isObject() &&
            config["nodeType"].isMember("message") && config["nodeType"]["message"].isString() &&
            config["nodeType"].isMember("gaming") && config["nodeType"]["gaming"].isString() &&
            config["nodeType"].isMember("transfer") && config["nodeType"]["transfer"].isString() &&
            config["nodeType"].isMember("user") && config["nodeType"]["user"].isString()
    )) {
        LOG_ERROR << R"(Invalid nodeType config)";
        abort();
    } else {
        _typeMapper[config["nodeType"]["gaming"].asString()] = NodeServer::Type::gaming;
        _typeMapper[config["nodeType"]["message"].asString()] = NodeServer::Type::message;
        _typeMapper[config["nodeType"]["transfer"].asString()] = NodeServer::Type::transfer;
        _typeMapper[config["nodeType"]["user"].asString()] = NodeServer::Type::user;
    }

    if (!(
            config.isMember("heartBeat") && config["heartBeat"].isObject() &&
            config["heartBeat"].isMember("waitTimes") && config["heartBeat"]["waitTimes"].isUInt()
    )) {
        LOG_ERROR << R"(Invalid heartBeat config)";
        abort();
    } else {
        _waitTimes = config["heartBeat"]["waitTimes"].asUInt();
    }

    LOG_INFO << "NodeManager loaded.";
}

void NodeManager::shutdown() { LOG_INFO << "NodeManager shutdown."; }

NodeServer::Type NodeManager::toType(const string &typeString) const {
    if (_typeMapper.contains(typeString)) {
        return _typeMapper.at(typeString);
    } else {
        throw std::runtime_error("Invalid node type");
    }
}

void NodeManager::updateNode(NodeServer &&nodeServer) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    auto[nodePair, isNew] = _allNodes[nodeServer.getType()].try_emplace(
            nodeServer.getNetEndian(),
            move(nodeServer)
    );
    if (!isNew) {
        nodePair->second.setInfo(move(nodeServer.getInfo()));
    }
    nodePair->second.updateLastSeen();
    updateTimer(nodePair->second);
}

string NodeManager::getBestNode(const NodeServer::Type &type) const {
    //TODO: implement logics
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _allNodes.at(type).begin()->second.getIpPort();
}

Json::Value NodeManager::parseInfo() const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto&[type, nodeList]: _allNodes) {
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

Json::Value NodeManager::parseInfo(const NodeServer::Type &nodeType) const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto&[type, nodeList]: _allNodes) {
        if (type == nodeType) {
            for (const auto &[_, node]: nodeList) {
                result.append(node.parseNode());
            }
            break;
        }
    }
    return result;
}

void NodeManager::updateTimer(NodeServer &nodeServer) {
    if (_waitTimes == 0) {
        // Disable heartbeat timeout
        return;
    }
    // Invalidate old heartbeat timeout
    app().getLoop()->invalidateTimer(nodeServer.getTimerId());

    const auto &type = nodeServer.getType();
    const auto &netEndian = nodeServer.getNetEndian();
    // Set new heartbeat timeout
    nodeServer.setTimerId(app().getLoop()->runAfter(
            nodeServer.getInterval() * _waitTimes,
            [this, type, netEndian]() {
                unordered_map<NetEndian, NodeServer>::node_type node; // Declare node first to avoid memory leak
                unique_lock<shared_mutex> lock(_sharedMutex);
                node = _allNodes[type].extract(netEndian);
                if (node.empty()) {
                    LOG_WARN << "Node " << netEndian << " already removed";
                }
            }
    ));
}
