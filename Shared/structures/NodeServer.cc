//
// Created by Particle_G on 2021/8/19.
//

#include <structures/NodeServer.h>

using namespace std;
using namespace tech::structures;
using namespace tech::types;
using namespace trantor;

NodeServer::NodeServer(
        const NodeType &type,
        const string &ip,
        const uint16_t &port,
        const double &taskInterval,
        string description,
        Json::Value info
) : _type(type),
    _inetAddress(ip, port),
    _taskInterval(taskInterval),
    _description(move(description)),
    _info(move(info)) {}

NodeServer::NodeServer(NodeServer &&nodeServer) noexcept:
        _inetAddress(nodeServer._inetAddress),
        _taskInterval(nodeServer._taskInterval),
        _type(nodeServer._type),
        _description(nodeServer._description),
        _info(nodeServer._info) {}

std::string NodeServer::getIpPort() const {
    return _inetAddress.toIpPort();
}

uint64_t NodeServer::getNetEndian() const {
    return (static_cast<uint64_t>(_inetAddress.ipNetEndian()) << 32) + _inetAddress.portNetEndian();
}

NodeType NodeServer::getType() const { return _type; }

double NodeServer::getInterval() const { return _taskInterval; }

std::string NodeServer::getDescription() const { return _description; }

Json::Value NodeServer::getInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _info;
}

void NodeServer::setInfo(Json::Value info) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _info = move(info);
}

uint64_t NodeServer::getTimerId() const {
    return _deactivateTimerId;
}

void NodeServer::setTimerId(const uint64_t &timerId) {
    _deactivateTimerId = timerId;
}

int64_t NodeServer::getLastSeen() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _lastSeen.microSecondsSinceEpoch();
}

void NodeServer::updateLastSeen() {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _lastSeen = Date::now();
}

Json::Value NodeServer::parseNode() const {
    Json::Value result;
    result["host"] = getIpPort();
    result["description"] = getDescription();
    result["info"] = getInfo();
    result["timerId"] = getTimerId();
    result["lastSeen"] = getLastSeen();
    return result;
}