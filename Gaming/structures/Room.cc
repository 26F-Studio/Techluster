//
// Created by particleg on 2021/10/8.
//

#include <structures/Exceptions.h>
#include <structures/Player.h>
#include <structures/Room.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace std;
using namespace tech::structures;
using namespace tech::utils;

Room::Room(
        const string &password,
        const uint64_t &capacity,
        Json::Value info,
        Json::Value data
) : _roomId(drogon::utils::getUuid()),
    _passwordHash(crypto::blake2b(password)),
    _capacity(capacity),
    _info(move(info)),
    _data(move(data)) {
    _state = State::pending;
}

Room::Room(Room &&room) noexcept:
        _roomId(room._roomId),
        _passwordHash(room._passwordHash),
        _capacity(room._capacity.load()),
        _info(move(room._info)),
        _data(move(room._data)) {
    _state = State::pending;
}

inline const string &Room::roomId() const { return _roomId; }

inline bool Room::checkPassword(const string &password) const {
    return crypto::blake2b(password) == _passwordHash;
}

Json::Value Room::getInfo(const Json::Value &list) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result;
    for (const auto &item: list) {
        auto &tempInput = _info;
        for (const auto &path: item) {
            if (path.isUInt()) {
                tempInput = tempInput[path.asUInt()];
            } else if (path.isString()) {
                tempInput = tempInput[path.asString()];
            } else {
                throw range_error("Invalid requirements list");
            }
        }
        result.append(tempInput);
    }
    return result;
}

void Room::setInfo(const Json::Value &list) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &item: list) {
        auto &tempInput = _info;
        for (const auto &path: item) {
            if (path.isUInt()) {
                tempInput = tempInput[path.asUInt()];
            } else if (path.isString()) {
                tempInput = tempInput[path.asString()];
            } else if (path.isObject()) {
                tempInput = path;
            } else {
                throw range_error("Invalid requirements list");
            }
        }
    }
}

Json::Value Room::getData(const Json::Value &list) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    Json::Value result;
    for (const auto &item: list) {
        auto &tempInput = _data;
        for (const auto &path: item) {
            if (path.isUInt()) {
                tempInput = tempInput[path.asUInt()];
            } else if (path.isString()) {
                tempInput = tempInput[path.asString()];
            } else {
                throw range_error("Invalid requirements list");
            }
        }
        result.append(tempInput);
    }
    return result;
}

void Room::setData(const Json::Value &list) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &item: list) {
        auto &tempInput = _data;
        for (const auto &path: item) {
            if (path.isUInt()) {
                tempInput = tempInput[path.asUInt()];
            } else if (path.isString()) {
                tempInput = tempInput[path.asString()];
            } else if (path.isObject()) {
                tempInput = path;
            } else {
                throw range_error("Invalid requirements list");
            }
        }
    }
}

Room::State Room::getState() const { return _state; }

void Room::setState(const Room::State &state) { _state = state; }

void Room::subscribe(const WebSocketConnectionPtr &connection) {
    _insert(connection);

    auto player = connection->getContext<Player>();
    if (_full() || _state == State::started) {
        player->setType(Player::Type::spectator);
    } else {
        player->setType(Player::Type::gamer);
        player->setState(Player::State::standby);
        if (_state == State::starting) {
            _cancelStarting();
        }
    }
}

inline uint64_t Room::unsubscribe(const WebSocketConnectionPtr &connection) {
    _remove(connection);
    return _size();
}

inline bool Room::empty() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.empty();
}

inline Json::Value Room::parse() const {
    Json::Value result;
    result["roomId"] = _roomId;
    result["capacity"] = _capacity.load();
    result["state"] = static_cast<int>(_state.load());
    result["players"] = Json::arrayValue;
    shared_lock<shared_mutex> lock(_sharedMutex);
    result["info"] = _info;
    result["data"] = _data;
    result["count"] = _connectionsMap.size();
    for (const auto &[_, connection]: _connectionsMap) {
        result["players"].append(connection->getContext<Player>()->info());
    }
    return result;
}

void Room::publish(
        string &&message,
        const int64_t &excludedId
) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (auto &pair: _connectionsMap) {
        if (excludedId < 0 ||
            excludedId != pair.second->getContext<Player>()->userId()) {
            pair.second->send(message);
        }
    }
}

void Room::checkReady() {
    shared_lock<shared_mutex> lock(_sharedMutex);
    if (_state != State::pending) {
        return;
    }
    for (const auto &[_, connection]: _connectionsMap) {
        const auto &player = connection->getContext<Player>();
        if (player->getType() == Player::Type::gamer ||
            player->getState() == Player::State::standby) {
            return;
        }
    }
    _startingGame();
}

inline bool Room::_full() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.size() == _capacity;
}

inline uint64_t Room::_size() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.size();
}

void Room::_insert(const WebSocketConnectionPtr &connection) {
    const auto &player = connection->getContext<Player>();
    if (!player->getJoinedId().empty()) {
        throw RoomException::PlayerOverFlow("Already in a room");
    }
    player->setJoinedId(roomId());
    unique_lock<shared_mutex> lock(_sharedMutex);
    _connectionsMap[player->userId()] = connection;
}

inline void Room::_remove(const WebSocketConnectionPtr &connection) {
    const auto &player = connection->getContext<Player>();
    if (player->getJoinedId() != roomId()) {
        throw RoomException::PlayerNotFound("Not in this room");
    }
    player->setJoinedId();
    unique_lock<shared_mutex> lock(_sharedMutex);
    _connectionsMap.erase(player->userId());
}

void Room::_startingGame() {
    app().getLoop()->runAfter(3, [this]() {
        setState(State::started);
        // TODO: Connect to transfer node
    });
}

inline void Room::_cancelStarting() {
    app().getLoop()->invalidateTimer(_timerId);
}

void Room::_checkFinished() {

}