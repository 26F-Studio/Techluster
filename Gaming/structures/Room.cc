//
// Created by particleg on 2021/10/8.
//

#include <atomic>
#include <magic_enum.hpp>
#include <plugins/NodeMaintainer.h>
#include <plugins/Authorizer.h>
#include <structures/Player.h>
#include <structures/Room.h>
#include <types/Action.h>
#include <types/MessageType.h>
#include <utils/crypto.h>


using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;
using namespace trantor;

Room::Room(
        uint64_t capacity,
        const string &password,
        Json::Value info,
        Json::Value data
) : capacity(capacity),
    _passwordHash(crypto::blake2B(password)),
    _info(move(info)),
    _data(move(data)),
    _connectionManager(app().getPlugin<ConnectionManager>()) {
    _data.canSkip(true);
    _data.canOverwrite(true);
    if (_data["endCondition"].isString()) {
        auto tempEndCondition = enum_cast<EndCondition>(_data["endCondition"].asString());
        if (tempEndCondition.has_value()) {
            endCondition = EndCondition{tempEndCondition.value()};
        }
    }

    switch (endCondition) {
        case EndCondition::GroupLeft:
            if (_data["leftLimit"].isUInt64()) {
                leftLimit = _data["leftLimit"].asUInt64() < capacity
                            ? _data["leftLimit"].asUInt64()
                            : capacity;
            }
            break;
        case EndCondition::Custom:
            LOG_ERROR << "Not implemented yet";
            [[fallthrough]];
        case EndCondition::PlayerLeft:
            if (_data["leftLimit"].isUInt64()) {
                leftLimit = _data["leftLimit"].asUInt64() < capacity
                            ? _data["leftLimit"].asUInt64()
                            : capacity;
            }
            break;
        case EndCondition::TimesUp: {
            uint64_t timeLimit = _data["timeLimit"].isUInt64()
                                 ? _data["timeLimit"].asUInt64()
                                 : 120;
            endTimerId = app().getLoop()->runAfter(
                    static_cast<double>(timeLimit),
                    [this]() { tryEnd(true); }
            );
            break;
        }
    }
}

Room::Room(Room &&room) noexcept:
        roomId(room.roomId),
        _passwordHash(move(room._passwordHash)),
        _info(move(room._info.ref())),
        _data(move(room._data.ref())),
        _playerSet(move(room._playerSet)),
        _connectionManager(room._connectionManager) {
    state = room.state.load();
    endCondition = room.endCondition.load();
    leftLimit = room.leftLimit.load();
    capacity = room.capacity.load();
    startTimerId = room.startTimerId.load();
    endTimerId = room.endTimerId.load();
    forwardingNode = room.forwardingNode.load();
}

bool Room::checkPassword(const string &password) const {
    return crypto::blake2B(password) == _passwordHash;
}

// TODO: Check if this would scramble the passwordHash
void Room::updatePassword(const string &password) {
    _passwordHash = crypto::blake2B(password);
}

void Room::subscribe(int64_t userId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _playerSet.insert(userId);
}

void Room::unsubscribe(int64_t userId) {
    unique_lock<shared_mutex> lock(_sharedMutex);
    _playerSet.erase(userId);
}

uint64_t Room::countGamer() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto userId: _playerSet) {
        const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
        if (player->type == Player::Type::Gamer) {
            counter++;
        }
    }
    return counter;
}

uint64_t Room::countGroup() const {
    unordered_set<uint64_t> groupCounter;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (const auto userId: _playerSet) {
            const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
            if (player->type == Player::Type::Gamer &&
                player->state == Player::State::Playing) {
                groupCounter.insert(player->group);
            }
        }
    }
    return groupCounter.size();
}

uint64_t Room::countPlaying() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto userId: _playerSet) {
        const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
        if (player->type == Player::Type::Gamer &&
            player->state == Player::State::Playing) {
            counter++;
        }
    }
    return counter;
}

uint64_t Room::countSpectator() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto userId: _playerSet) {
        const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
        if (player->type == Player::Type::Spectator) {
            counter++;
        }
    }
    return counter;
}

uint64_t Room::countStandby() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto userId: _playerSet) {
        const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
        if (player->type == Player::Type::Gamer &&
            player->state == Player::State::Standby) {
            counter++;
        }
    }
    return counter;
}

bool Room::empty() const { return countGamer() == 0; }

bool Room::full() const { return countGamer() >= capacity; }

Json::Value Room::parse(bool details) const {
    Json::Value result;
    result["roomId"] = roomId;
    result["capacity"] = capacity.load();
    result["state"] = string(enum_name(state.load()));
    result["count"]["gamer"] = countGamer();
    result["count"]["spectator"] = countSpectator();

    shared_lock<shared_mutex> lock(_sharedMutex);
    result["info"] = _info.copy();
    if (details) {
        result["data"] = _data.copy();
        result["players"] = Json::arrayValue;
        for (const auto userId: _playerSet) {
            result["players"].append(
                    _connectionManager->getConnPtr(userId)->getContext<Player>()->info()
            );
        }
    }
    return result;
}

void Room::publish(const MessageJson &message, int64_t excludedId) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto userId: _playerSet) {
        if (excludedId != userId) {
            message.sendTo(_connectionManager->getConnPtr(userId));
        }
    }
}

Json::Value Room::getData() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _data.copy();
}

Json::Value Room::updateData(const Json::Value &data) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &item: data) {
        _data.modifyByPath(item["path"].asString(), item["value"]);
    }
    return _data.copy();
}

Json::Value Room::getInfo() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _info.copy();
}

Json::Value Room::updateInfo(const Json::Value &data) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &item: data) {
        _info.modifyByPath(item["path"].asString(), item["value"]);
    }
    return _info.copy();
}

bool Room::tryCancelStart() {
    if (state == State::Ready) {
        app().getLoop()->invalidateTimer(startTimerId.load());
        state = State::Standby;
        return true;
    }
    return false;
}

void Room::tryEnd(bool force) {
    if (state != State::Playing) {
        return;
    }
    switch (endCondition) {
        case EndCondition::GroupLeft:
            if (countGroup() > leftLimit) {
                return;
            }
            break;
        case EndCondition::Custom:
            // TODO: Implement custom condition
            [[fallthrough]];
        case EndCondition::PlayerLeft:
            if (countPlaying() > leftLimit) {
                return;
            }
            break;
        case EndCondition::TimesUp:
            if (!force && countPlaying() > 1) {
                return;
            }
            break;
    }
    state = State::Standby;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (auto &userId: _playerSet) {
            _connectionManager->getConnPtr(
                    userId
            )->getContext<Player>()->state = Player::State::Standby;
        }
    }

    MessageJson message;
    message.setMessageType(MessageType::Server);
    message.setAction(enum_integer(Action::GameEnd));
    publish(message);

    _removeTransmission();
}

void Room::tryStart() {
    if (state != State::Standby || countStandby() > 0) {
        return;
    }

    state = State::Ready;

    MessageJson message;
    message.setMessageType(MessageType::Server);
    message.setAction(enum_integer(Action::GameReady));
    publish(message);

    startTimerId = app().getLoop()->runAfter(3, [this]() {
        state = State::Playing;
        _estimateForwardingNode();
        _createTransmission();

        Json::Value data;
        data["transferNode"] = forwardingNode.load().toIpPort();

        MessageJson message;
        message.setMessageType(MessageType::Server);
        message.setAction(enum_integer(Action::GameStart));
        message.setData(data);
        publish(message);
    });
}

Room::~Room() {
    tryCancelStart();
    for (const auto userId: _playerSet) {
        const auto &wsConnPtr = _connectionManager->getConnPtr(userId);
        wsConnPtr->getContext<Player>()->reset();
        unsubscribe(userId);

        MessageJson successMessage(enum_integer(Action::RoomRemove));
        successMessage.setMessageType(MessageType::Server);
        successMessage.sendTo(wsConnPtr);
    }
}

void Room::_estimateForwardingNode() {
    vector<Json::Value> pingLists;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (const auto userId: _playerSet) {
            const auto player = _connectionManager->getConnPtr(userId)->getContext<Player>();
            // TODO: Kick high delay spectators, replace high delay players with robots
            if (player->type == Player::Type::Gamer) {
                const auto &wsConnPtr = _connectionManager->getConnPtr(userId);
                pingLists.emplace_back(wsConnPtr->getContext<Player>()->getPingList());
            }
        }
    }

    // TODO: Implement estimating algorithm
    const auto &address = pingLists[0][0]["address"].asString();
    auto parts = drogon::utils::splitString(address, ":");
    if (parts.size() != 2) {
        throw NetworkException(
                "Invalid address: " + address,
                ReqResult::BadResponse
        );
    }
    forwardingNode = InetAddress(parts[0], stoi(parts[1]));
}

void Room::_createTransmission() {
    Json::Value body;
    body["roomId"] = roomId;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        for (const auto userId: _playerSet) {
            body["players"].append(userId);
        }
    }
    auto client = HttpClient::newHttpClient("http://" + forwardingNode.load().toIpPort());
    auto req = HttpRequest::newHttpJsonRequest(body);
    req->setMethod(Post);
    req->setPath("/tech/api/v2/Manage/create");
    auto [result, responsePtr] = client->sendRequest(req, 3);

    if (result != ReqResult::Ok || responsePtr->getStatusCode() != k200OK) {
        throw NetworkException("Connect node is down", result);
    }
}

void Room::_removeTransmission() {
    Json::Value body;
    body["roomId"] = roomId;
    auto client = HttpClient::newHttpClient("http://" + forwardingNode.load().toIpPort());
    auto req = HttpRequest::newHttpJsonRequest(body);
    req->setMethod(Post);
    req->setPath("/tech/api/v2/Manage/remove");
    auto [result, responsePtr] = client->sendRequest(req, 3);

    if (result != ReqResult::Ok || responsePtr->getStatusCode() != k200OK) {
        throw NetworkException("Connect node is down", result);
    }
}
