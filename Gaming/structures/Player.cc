//
// Created by particleg on 2021/10/8.
//

#include <structures/Player.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Player::Player(const int64_t &userId) : _userId(userId) {
    _group = 0;
    _role = Role::normal;
    _type = Type::gamer;
    _state = State::standby;
}

const int64_t &Player::userId() const { return _userId; }

uint32_t Player::getGroup() const { return _group; }

void Player::setGroup(const uint32_t &group) { _group = group; }

Player::Role Player::getRole() const { return _role; }

void Player::setRole(const Player::Role &role) { _role = role; }

Player::Type Player::getType() const { return _type; }

void Player::setType(const Player::Type &type) { _type = type; }

Player::State Player::getState() const { return _state; }

void Player::setState(const Player::State &state) { _state = state; }

string Player::getConfig() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _config;
}

void Player::setConfig(string &&config) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _config = move(config);
}

string Player::getJoinedId() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _joinedId;
}

void Player::setJoinedId(const string &joinedId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _joinedId = joinedId;
}

Json::Value Player::info() const {
    Json::Value result;
    result["userId"] = userId();
    result["group"] = getGroup();
    result["role"] = static_cast<int>(getRole());
    result["type"] = static_cast<int>(getType());
    result["state"] = static_cast<int>(getState());
    result["config"] = getConfig();

    return result;
}

void Player::reset() {
    _group = 0;
    _role = Role::normal;
    _type = Type::gamer;
    _state = State::standby;

    setJoinedId();
}
