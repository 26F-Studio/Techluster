//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <structures/Player.h>
#include <utils/crypto.h>

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
        unique_lock<shared_mutex> innerLock(*iter->second.sharedMutex);
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
                iter->second.room,
                move(shared_lock<shared_mutex>(
                        *iter->second.sharedMutex
                ))
        );
    }
    throw room_exception::RoomNotFound("Invalid room id");
}

RoomManager::UniqueRoom RoomManager::getUniqueRoom(const string &roomId) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    auto iter = _roomMap.find(roomId);
    if (iter != _roomMap.end()) {
        return UniqueRoom(
                iter->second.room,
                move(unique_lock<shared_mutex>(
                        *iter->second.sharedMutex
                ))
        );
    }
    throw room_exception::RoomNotFound("Invalid room id");
}

void RoomManager::joinRoom(
        const WebSocketConnectionPtr &connection,
        const string &roomId,
        const string &password
) {
    auto sharedRoom = getSharedRoom(roomId);
    auto &room = sharedRoom.room;
    if (!room.checkPassword(password)) {
        throw room_exception::InvalidPassword("Password is incorrect");
    }

    room.subscribe(connection);
}

void RoomManager::leaveRoom(
        const WebSocketConnectionPtr &connection
) {
    const auto &player = connection->getContext<Player>();
    auto roomId = player->getJoinedId();
    bool isEmpty;
    {
        auto sharedRoom = getSharedRoom(roomId);
        auto &room = sharedRoom.room;
        room.unsubscribe(connection);
        isEmpty = room.empty();
    }
    if (isEmpty) { // TODO: Check if thread safe
        removeRoom(roomId);
    }
}

Json::Value RoomManager::roomList(
        const string &search,
        const uint64_t &begin,
        const uint64_t &count
) const {
    Json::Value result(Json::arrayValue);
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (begin < _roomMap.size()) {
        uint64_t counter{};
        for (const auto&[roomId, roomWithMutex]: _roomMap) {
            if (counter < begin) {
                ++counter;
                continue;
            }
            if (counter >= begin + count) {
                break;
            }
            if (search.empty() || roomId.find(search) != string::npos) {
                shared_lock<shared_mutex> innerLock(*roomWithMutex.sharedMutex);
                result.append(roomWithMutex.room.parse());
            }
            ++counter;
        }
    }
    return result;
}

RoomManager::RoomWithMutex::RoomWithMutex(
        Room &&room
) : room(move(room)),
    sharedMutex(new shared_mutex()) {}

RoomManager::RoomWithMutex::RoomWithMutex(
        RoomManager::RoomWithMutex &&moved
) noexcept: room(move(moved.room)),
            sharedMutex(move(moved.sharedMutex)) {}