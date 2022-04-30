//
// Created by particleg on 2021/10/8.
//

#include <magic_enum.hpp>
#include <structures/Player.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace magic_enum;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Player::Player(
        int64_t userId,
        Role role,
        State state,
        Type type
) : BasicPlayer(userId),
    role(role),
    state(state),
    type(type) {}

Player::Player(Player &&player) noexcept:
        BasicPlayer(player.userId),
        _customState(move(player._customState)),
        _config(move(player._config)) {
    group = player.group.load();
    role = player.role.load();
    type = player.type.load();
    state = player.state.load();
}

string Player::getRoomId() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _roomId;
}

void Player::setRoomId(const string &roomId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _roomId = roomId;
}

string Player::getCustomState() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _customState;
}

void Player::setCustomState(string &&customState) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _customState = move(customState);
}

string Player::getConfig() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _config;
}

void Player::setConfig(string &&config) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _config = move(config);
}

Json::Value Player::getPingList() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _pingList;
}

void Player::setPingList(Json::Value &&pingList) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _pingList = move(pingList);
}

Json::Value Player::info() const {
    Json::Value result;
    result["userId"] = userId;
    result["group"] = group.load();
    result["role"] = string(enum_name(role.load()));
    result["type"] = string(enum_name(type.load()));
    result["config"] = getConfig();

    shared_lock<shared_mutex> lock(_sharedMutex);
    if (state == State::playing) {
        result["state"] = _customState;
    } else {
        result["state"] = string(enum_name(state.load()));
    }
    return result;
}

void Player::reset() {
    group = 0;
    role = Role::normal;
    state = State::standby;
    type = Type::spectator;
    unique_lock<shared_mutex> lock(_sharedMutex);
    _roomId.clear();
    _customState.clear();
}
