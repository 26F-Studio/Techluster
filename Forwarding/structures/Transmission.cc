//
// Created by particleg on 2021/10/8.
//

#include <magic_enum.hpp>
#include <plugins/NodeMaintainer.h>
#include <plugins/Authorizer.h>
#include <structures/Transmitter.h>
#include <structures/Transmission.h>
#include <types/Action.h>
#include <types/MessageType.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

Transmission::Transmission(string &&roomId, set<int64_t> &&playerSet) :
        roomId(move(roomId)),
        playerSet(move(playerSet)),
        _connectionManager(app().getPlugin<ConnectionManager>()) {}

Transmission::Transmission(Transmission &&transmission) noexcept:
        roomId(transmission.roomId),
        playerSet(transmission.playerSet),
        _connectionManager(transmission._connectionManager) {}

void Transmission::join(int64_t userId) {
    const auto &transmitter = _connectionManager->getConnPtr(userId)->getContext<Transmitter>();
    if (playerSet.contains(userId)) {
        transmitter->type = Transmitter::Type::gamer;
    }
    {
        unique_lock<shared_mutex> lock(_transmitterMutex);
        _transmitterSet.insert(userId);
    }
    transmitter->setRoomId(roomId);
    {
        shared_lock<shared_mutex> lock(_transmitterMutex);
        if (includes(_transmitterSet.begin(), _transmitterSet.end(), playerSet.begin(), playerSet.end())) {
            MessageJson publishMessage(enum_integer(Action::TransmissionStart));
            publishMessage.setMessageType(MessageType::Server);
            publish(publishMessage);
        }
    }
}

void Transmission::leave(int64_t userId) {
    const auto &transmitter = _connectionManager->getConnPtr(userId)->getContext<Transmitter>();
    {
        unique_lock<shared_mutex> lock(_transmitterMutex);
        _transmitterSet.erase(userId);
    }
    transmitter->reset();
}

void Transmission::publish(const MessageJson &message, int64_t excludedId) const {
    shared_lock<shared_mutex> lock(_transmitterMutex);
    for (const auto &userId: _transmitterSet) {
        if (excludedId != userId) {
            message.sendTo(_connectionManager->getConnPtr(userId));
        }
    }
}

void Transmission::history(int64_t userId, string &&message) {
    unique_lock<shared_mutex> lock(_historyMutex);
    _history[userId].append(message);
}

Json::Value Transmission::parse() const {
    Json::Value result;
    result["roomId"] = roomId;
    result["playerSet"] = Json::arrayValue;

    shared_lock<shared_mutex> transmitterLock(_transmitterMutex), historyLock(_historyMutex);
    for (const auto &userId: _transmitterSet) {
        Json::Value info(_connectionManager->getConnPtr(userId)->getContext<Transmitter>()->info());
        info["history"] = _history.contains(userId) ? _history.at(userId) : "";
        result["transmitters"].append(move(info));
    }
    return result;
}

Transmission::~Transmission() {
    for (const auto &userId: _transmitterSet) {
        _connectionManager->getConnPtr(userId)->getContext<Transmitter>()->reset();
    }
}
