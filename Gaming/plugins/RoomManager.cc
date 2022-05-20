//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <magic_enum.hpp>
#include <helpers/MessageJson.h>
#include <plugins/RoomManager.h>
#include <structures/Exceptions.h>
#include <structures/Player.h>
#include <types/Action.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

RoomManager::RoomManager() = default;

void RoomManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "RoomManager loaded.";
}

void RoomManager::shutdown() { LOG_INFO << "RoomManager shutdown."; }

void RoomManager::playerConfig(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();

    Json::Value data;
    data["userId"] = player->userId;
    data["config"] = player->getConfig();
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        _roomMap.at(player->getRoomId()).publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerFinish(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        Json::Value &&finishData
) {
    const auto &player = wsConnPtr->getContext<Player>();

    Json::Value data;
    data["userId"] = player->userId;
    data["finishData"] = move(finishData);
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(player->getRoomId());
        room.publish(publishMessage);
        room.tryEnd();
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerGroup(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();

    Json::Value data;
    data["userId"] = player->userId;
    data["group"] = player->group.load();
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        _roomMap.at(player->getRoomId()).publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerReady(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();
    auto ready = player->state == Player::State::ready;

    Json::Value data;
    data["userId"] = player->userId;
    data["ready"] = ready;
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(player->getRoomId());
        room.publish(publishMessage);
        if (ready) {
            room.tryStart();
        } else {
            room.tryCancelStart();
        }
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerRole(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();

    Json::Value data;
    data["userId"] = player->userId;
    data["role"] = string(enum_name(player->role.load()));
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        _roomMap.at(player->getRoomId()).publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerState(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();

    Json::Value data;
    data["userId"] = player->userId;
    data["customState"] = player->getCustomState();
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        _roomMap.at(player->getRoomId()).publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::playerType(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        Player::Type type
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(player->getRoomId());
        if (type == Player::Type::gamer && room.full()) {
            MessageJson failedMessage(action);
            failedMessage.setMessageType(MessageType::Failed);
            failedMessage.setReason(i18n("roomFull"));
            failedMessage.sendTo(wsConnPtr);
        } else {
            player->type = type;

            Json::Value data;
            data["userId"] = player->userId;
            data["type"] = string(enum_name(player->type.load()));
            MessageJson publishMessage(action);
            publishMessage.setData(move(data));
            room.publish(publishMessage);
        }
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomCreate(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        uint32_t capacity,
        string &&password,
        Json::Value roomInfo,
        Json::Value roomData
) {
    const auto &player = wsConnPtr->getContext<Player>();

    Room room(
            capacity,
            password,
            move(roomInfo),
            move(roomData)
    );
    const auto &roomId = room.roomId;
    room.subscribe(player->userId);
    player->setRoomId(roomId);
    player->role = Player::Role::admin;
    {
        unique_lock<shared_mutex> lock(_sharedMutex);
        _roomMap.emplace(roomId, move(room));
    }

    MessageJson publishMessage(action);
    publishMessage.setData(roomId);
    publishMessage.sendTo(wsConnPtr);
}

void RoomManager::roomDataGet(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        const string &roomId
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        MessageJson successMessage(action);
        successMessage.setData(_roomMap.at(roomId).getData());
        successMessage.sendTo(wsConnPtr);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomDataUpdate(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        const string &roomId,
        const Json::Value &data
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(roomId);
        MessageJson publishMessage(action);
        publishMessage.setMessageType(MessageType::Server);
        publishMessage.setData(room.updateData(data));
        room.publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomInfoGet(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        const string &roomId
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        MessageJson successMessage(action);
        successMessage.setData(_roomMap.at(roomId).getInfo());
        successMessage.sendTo(wsConnPtr);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomInfoUpdate(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        const string &roomId,
        const Json::Value &data
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(player->getRoomId());
        MessageJson publishMessage(action);
        publishMessage.setMessageType(MessageType::Server);
        publishMessage.setData(room.updateInfo(data));
        room.publish(publishMessage);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomJoin(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        std::string &&roomId,
        std::string &&password
) {
    const auto &player = wsConnPtr->getContext<Player>();
    const auto &userId = player->userId;
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(roomId);
        if (room.checkPassword(password)) {
            room.subscribe(userId);
            player->setRoomId(roomId);

            bool spectate = false;
            if (room.state == Room::State::playing) {
                spectate = true;
            } else if (!room.full() && room.tryCancelStart()) {
                player->type = Player::Type::gamer;
            }

            MessageJson successMessage(action);
            successMessage.setMessageType(MessageType::Server);
            successMessage.setData(room.parse(true));
            successMessage.sendTo(wsConnPtr);

            MessageJson publishMessage(action);
            publishMessage.setData(player->info());
            room.publish(publishMessage, userId);

            if (spectate) {
                MessageJson spectateMessage(enum_integer(Action::GameSpectate));
                spectateMessage.setData(room.forwardingNode.load().toIpPort());
                spectateMessage.sendTo(wsConnPtr);
            }
        } else {
            throw MessageException("wrongPassword");
        }
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
}

void RoomManager::roomKick(int action, const WebSocketConnectionPtr &wsConnPtr) {
    roomLeave(action, wsConnPtr);
}

void RoomManager::roomLeave(int action, const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();
    const auto &roomId = player->getRoomId();
    const auto &userId = player->userId;

    Json::Value data;
    data["userId"] = userId;
    MessageJson publishMessage(action);
    publishMessage.setData(move(data));
    bool empty;
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(roomId);
        room.unsubscribe(userId);
        room.publish(publishMessage, userId);
        player->reset();

        empty = room.empty();
        if (!empty) {
            room.tryEnd();
        }
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }

    MessageJson successMessage(action);
    successMessage.setMessageType(MessageType::Server);
    successMessage.sendTo(wsConnPtr);

    if (empty) {
        unique_lock<std::shared_mutex> lock(_sharedMutex);
        _roomMap.erase(roomId);
    }
}

void RoomManager::roomList(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        string &&search,
        uint64_t begin,
        uint64_t count
) const {
    Json::Value data(Json::arrayValue);
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        if (begin < _roomMap.size()) {
            uint64_t counter{};
            for (const auto &[roomId, room]: _roomMap) {
                if (counter < begin) {
                    ++counter;
                    continue;
                }
                if (counter >= begin + count) {
                    break;
                }
                if (search.empty() || roomId.find(search) != string::npos) {
                    data.append(room.parse());
                }
                ++counter;
            }
        }
    }
    MessageJson successMessage(action);
    successMessage.setData(move(data));
    successMessage.sendTo(wsConnPtr);
}

void RoomManager::roomPassword(
        int action,
        const WebSocketConnectionPtr &wsConnPtr,
        string &&password
) {
    const auto &player = wsConnPtr->getContext<Player>();
    try {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto &room = _roomMap.at(player->getRoomId());

        room.updatePassword(password);
    } catch (const out_of_range &) {
        throw MessageException("roomNotFound");
    }
    MessageJson successMessage(action);
    successMessage.setMessageType(MessageType::Server);
    successMessage.sendTo(wsConnPtr);
}

void RoomManager::roomRemove(const WebSocketConnectionPtr &wsConnPtr) {
    const auto &player = wsConnPtr->getContext<Player>();
    {
        unique_lock<std::shared_mutex> lock(_sharedMutex);
        _roomMap.erase(player->getRoomId());
    }
}