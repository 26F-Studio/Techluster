//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <magic_enum.hpp>
#include <helpers/MessageJson.h>
#include <plugins/TransmissionManager.h>
#include <structures/Transmitter.h>
#include <types/Action.h>
#include <types/MessageType.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void TransmissionManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "TransferManager loaded.";
}

void TransmissionManager::shutdown() { LOG_INFO << "TransferManager shutdown."; }

void TransmissionManager::create(string &&roomId, set<int64_t> &&playerSet) {
    Transmission transmission(move(roomId), move(playerSet));
    unique_lock<shared_mutex> lock(_sharedMutex);
    _transmissionMap.emplace(transmission.roomId, move(transmission));
}

void TransmissionManager::remove(const string &roomId) {
    MessageJson publishMessage(enum_integer(Action::transmissionRemove));
    publishMessage.setMessageType(MessageType::server);
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        _transmissionMap.at(roomId).publish(publishMessage);
    }
    {
        unique_lock<shared_mutex> lock(_sharedMutex);
        _transmissionMap.erase(roomId);
    }
}

void TransmissionManager::transmissionBroadcast(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        string &&message
) {
    const auto &transmitter = wsConnPtr->getContext<Transmitter>();
    MessageJson publishMessage(action);
    publishMessage.setData(message);
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _transmissionMap.at(transmitter->getRoomId());
        room.publish(publishMessage);
        room.history(transmitter->userId, move(message));
    }
}

void TransmissionManager::transmissionJoin(int action, const WebSocketConnectionPtr &wsConnPtr, string &&roomId) {
    const auto &transmitter = wsConnPtr->getContext<Transmitter>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _transmissionMap.at(roomId);
        room.join(transmitter->userId);

        MessageJson publishMessage(action);
        publishMessage.setData(transmitter->info());
        room.publish(publishMessage, transmitter->userId);

        MessageJson selfMessage(action);
        selfMessage.setMessageType(MessageType::server);
        selfMessage.setData(room.parse());
        selfMessage.sendTo(wsConnPtr);
    } catch (const out_of_range &e) {
        throw MessageException("roomNotFound");
    }
}

void TransmissionManager::transmissionLeave(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &transmitter = wsConnPtr->getContext<Transmitter>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _transmissionMap.at(transmitter->getRoomId());
        room.leave(transmitter->userId);

        MessageJson publishMessage(action);
        publishMessage.setData(transmitter->info());
        room.publish(publishMessage, transmitter->userId);
    } catch (const out_of_range &e) {
        throw MessageException("roomNotFound");
    }

    MessageJson selfMessage(action);
    selfMessage.setMessageType(MessageType::server);
    selfMessage.sendTo(wsConnPtr);
}
