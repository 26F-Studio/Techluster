//
// Created by particleg on 2021/10/8.
//

#include <structures/Player.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Player::Player(const int64_t &id) : _id(id) {
    _group = 0;
    _role = Role::normal;
    _type = Type::gamer;
    _state = State::standby;
}

inline const int64_t &Player::userId() const { return _id; }

inline uint32_t Player::getGroup() const { return _group; }

inline void Player::setGroup(const uint32_t &group) { _group = group; }

inline Player::Role Player::getRole() const { return _role; }

inline void Player::setRole(const Player::Role &role) { _role = role; }

inline Player::Type Player::getType() const { return _type; }

inline void Player::setType(const Player::Type &type) { _type = type; }

inline Player::State Player::getState() const { return _state; }

inline void Player::setState(const Player::State &state) { _state = state; }

inline string Player::getConfig() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _config;
}

inline void Player::setConfig(string &&config) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _config = move(config);
}

inline string Player::getJoinedId() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _joinedId;
}

inline void Player::setJoinedId(const string &joinedId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _joinedId = joinedId;
}

inline Json::Value Player::info() const {
    Json::Value result;
    result["id"] = userId();
    result["group"] = getGroup();
    result["role"] = static_cast<int>(getRole());
    result["type"] = static_cast<int>(getType());
    result["state"] = static_cast<int>(getState());
    result["config"] = getConfig();

    return result;
}
