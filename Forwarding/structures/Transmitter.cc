//
// Created by particleg on 2021/10/8.
//

#include <magic_enum.hpp>
#include <structures/Transmitter.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace magic_enum;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Transmitter::Transmitter(int64_t userId) : BasicPlayer(userId) {}

Transmitter::Transmitter(Transmitter &&transmitter) noexcept:
        BasicPlayer(transmitter.userId) {
    type = transmitter.type.load();
}

string Transmitter::getRoomId() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _roomId;
}

void Transmitter::setRoomId(const string &roomId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _roomId = roomId;
}

Json::Value Transmitter::info() const {
    Json::Value result;
    result["userId"] = userId;
    result["type"] = string(enum_name(type.load()));
    return result;
}

void Transmitter::reset() {
    type = Type::Spectator;
    shared_lock<shared_mutex> lock(_sharedMutex);
    _roomId.clear();
}
