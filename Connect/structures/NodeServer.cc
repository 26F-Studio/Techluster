//
// Created by Particle_G on 2021/8/19.
//

#include <structures/NodeServer.h>

using namespace std;
using namespace tech::structures;

NodeServer::NodeServer(
        string host,
        Type type,
        string description
) : _host(move(host)), _type(type), _description(move(description)) {
    _info["CPU"] = "";
    _info["Memory"] = "";
    _info["Disk"] = "";
    _info["Traffic"] = "";
    _info["Connections"] = "";
}

NodeServer::NodeServer(NodeServer &&nodeServer) noexcept:
        _host(nodeServer._host),
        _type(nodeServer._type),
        _description(nodeServer._description),
        _info(nodeServer._info) {
    _failureCount = nodeServer.getFailureCount();
}

std::string NodeServer::getHost() const { return _host; }

std::string NodeServer::getDescription() const { return _description; }

NodeServer::Type NodeServer::getType() const { return _type; }

Json::Value NodeServer::getInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _info;
}

void NodeServer::setInfo(Json::Value info) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _info = move(info);
}

uint64_t NodeServer::getFailureCount() const { return _failureCount; }

void NodeServer::incrementFailureCount() { _failureCount++; }

void NodeServer::clearFailureCount() { _failureCount = 0; }

