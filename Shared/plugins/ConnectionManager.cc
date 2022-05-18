//
// Created by Parti on 2021/2/4.
//

#include <plugins/ConnectionManager.h>
#include <structures/Exceptions.h>
#include <structures/BasicPlayer.h>

using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;

void ConnectionManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "ConnectionManager loaded.";
}

void ConnectionManager::shutdown() { LOG_INFO << "ConnectionManager shutdown."; }

void ConnectionManager::subscribe(const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<BasicPlayer>();
    if (wsConnPtr->connected() && player) {
        const auto userId = player->userId;
        unique_lock<shared_mutex> lock(_sharedMutex);
        if (_connectionMap.contains(userId) &&
            _connectionMap[userId]->connected()) {
            MessageJson message;
            message.setMessageType(MessageType::Error);
            message.setReason(i18n("connectionReplaced"));
            message.closeWith(_connectionMap[userId]);
        }
        _connectionMap[userId] = wsConnPtr;
    }
}

void ConnectionManager::unsubscribe(const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<BasicPlayer>();
    if (wsConnPtr->connected() && player) {
        try {
            unique_lock<shared_mutex> lock(_sharedMutex);
            if (_connectionMap.at(player->userId) == wsConnPtr) {
                _connectionMap.erase(player->userId);
            } else {
                LOG_DEBUG << "Unsubscribe failed, not same connection: \n"
                          << "\tOriginal: (local: "
                          << _connectionMap.at(player->userId)->localAddr().toIpPort()
                          << ", remote: "
                          << _connectionMap.at(player->userId)->peerAddr().toIpPort()
                          << ")\n"
                          << "\tCurrent: (local: "
                          << wsConnPtr->localAddr().toIpPort()
                          << ", remote: "
                          << wsConnPtr->peerAddr().toIpPort()
                          << ")";
            }
        } catch (const out_of_range &e) {
            throw MessageException("playerNotFound");
        }
    }
}

WebSocketConnectionPtr ConnectionManager::getConnPtr(int64_t userId) {
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        const auto &wsConnPtr = _connectionMap.at(userId);
        if (!wsConnPtr->connected() ||
            !wsConnPtr->getContext<BasicPlayer>()) {
            unsubscribe(wsConnPtr);
            throw MessageException(i18n("playerInvalid"));
        }
        return wsConnPtr;
    } catch (const out_of_range &e) {
        throw MessageException("playerNotFound");
    }
}
