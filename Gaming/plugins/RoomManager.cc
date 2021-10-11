//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <structures/Exceptions.h>
#include <structures/MessageHandler.h>
#include <structures/Player.h>
#include <utils/crypto.h>
#include <utils/serializer.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::utils;

RoomManager::SharedRoom::SharedRoom(
        Room &room,
        shared_lock<shared_mutex> &&lock
) : room(room), _lock(move(lock)) {}

RoomManager::UniqueRoom::UniqueRoom(
        Room &room,
        unique_lock<shared_mutex> &&lock
) : room(room), _lock(move(lock)) {}

void RoomManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "RoomManager loaded.";
}

void RoomManager::shutdown() { LOG_INFO << "RoomManager shutdown."; }

string RoomManager::createRoom(
        const string &password,
        const uint32_t &capacity,
        const Json::Value &info,
        const Json::Value &data
) {
    auto room = Room(
            password,
            capacity,
            info,
            data
    );
    auto roomId = room.roomId();
    {
        unique_lock<shared_mutex> lock(_sharedMutex);
        auto[_, result]=_roomMap.try_emplace(
                roomId,
                RoomWithMutex(move(room))
        );
        if (!result) {
            LOG_FATAL << "UUID collided!";
            return {};
        }
    }
    return roomId;
}

void RoomManager::removeRoom(const string &roomId) {
    unique_lock<std::shared_mutex> lock(_sharedMutex);
    auto iter = _roomMap.find(roomId);
    if (iter != _roomMap.end()) {
        typename decltype(_roomMap)::node_type node; // Declare node first to avoid memory leak
        unique_lock<shared_mutex> innerLock(*iter->second._sharedMutex);
        node = _roomMap.extract(roomId);
        if (node.empty()) {
            LOG_INFO << "Room " << roomId << " already removed";
        }
    } else {
        LOG_INFO << "Room " << roomId << " does not exist";
    }
}

RoomManager::SharedRoom RoomManager::getSharedRoom(const string &roomId) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    auto iter = _roomMap.find(roomId);
    if (iter != _roomMap.end()) {
        return SharedRoom(
                iter->second._room,
                move(shared_lock<shared_mutex>(
                        *iter->second._sharedMutex
                ))
        );
    }
    throw RoomException::RoomNotFound("Invalid room id");
}

RoomManager::UniqueRoom RoomManager::getUniqueRoom(const string &roomId) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    auto iter = _roomMap.find(roomId);
    if (iter != _roomMap.end()) {
        return UniqueRoom(
                iter->second._room,
                move(unique_lock<shared_mutex>(
                        *iter->second._sharedMutex
                ))
        );
    }
    throw RoomException::RoomNotFound("Invalid room id");
}

void RoomManager::joinRoom(
        const string &roomId,
        const string &password,
        const WebSocketConnectionPtr &connection
) {
    auto &room = getSharedRoom(roomId).room;
    if (!room.checkPassword(password)) {
        throw RoomException::InvalidPassword("Password is incorrect");
    }

    const auto &player = connection->getContext<Player>();

    room.subscribe(connection);

    Json::Value message, response;
    message["origin"] = static_cast<int>(Origin::other);
    message["action"] = toUInt(Action::roomJoin);
    message["data"] = player->info();
    room.publish(
            move(serializer::json::stringify(message)),
            player->userId()
    );

    response["type"] = static_cast<int>(Origin::self);
    response["action"] = toUInt(Action::roomJoin);
    response["data"] = room.parse();
    connection->send(serializer::json::stringify(response));
}

void RoomManager::leaveRoom(
        const WebSocketConnectionPtr &connection
) {
    if (connection->connected()) {
        Json::Value response;
        response["type"] = static_cast<int>(Origin::self);
        response["action"] = static_cast<int>(Action::roomLeave);
        connection->send(serializer::json::stringify(response));
    }

    const auto &player = connection->getContext<Player>();
    auto roomId = player->getJoinedId();
    bool isEmpty;
    {
        auto &room = getSharedRoom(roomId).room;
        if (room.unsubscribe(connection)) {
            Json::Value message;
            message["type"] = static_cast<int>(Origin::other);
            message["action"] = static_cast<int>(Action::roomLeave);
            message["data"] = player->info();
            room.publish(move(serializer::json::stringify(message)));
        }
        isEmpty = room.empty();
    }
    if (isEmpty) { // TODO: Check if thread safe
        removeRoom(roomId);
    }
}

RoomManager::RoomWithMutex::RoomWithMutex(
        Room &&room
) : _room(move(room)),
    _sharedMutex(new shared_mutex()) {}

RoomManager::RoomWithMutex::RoomWithMutex(
        RoomManager::RoomWithMutex &&moved
) noexcept: _room(move(moved._room)),
            _sharedMutex(move(moved._sharedMutex)) {}