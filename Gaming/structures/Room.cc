//
// Created by particleg on 2021/10/8.
//

#include <plugins/AuthMaintainer.h>
#include <plugins/Authorizer.h>
#include <strategies/Action.h>
#include <structures/MessageHandler.h>
#include <structures/Player.h>
#include <structures/Room.h>
#include <utils/crypto.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::utils;

Room::Room(
        const string &password,
        const uint64_t &capacity,
        Json::Value info,
        Json::Value data
) : _roomId(drogon::utils::getUuid()),
    _passwordHash(crypto::blake2B(password)),
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

const string &Room::roomId() const { return _roomId; }

bool Room::checkPassword(const string &password) const {
    return crypto::blake2B(password) == _passwordHash;
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

void Room::subscribe(const WebSocketConnectionPtr &connection) {
    _insert(connection);

    auto player = connection->getContext<Player>();
    if (_full() || _state == State::started) {
        player->setType(Player::Type::spectator);
    } else {
        player->setType(Player::Type::gamer);
        player->setState(Player::State::standby);
        if (_state == State::starting) {
            cancelStarting();
        }
    }

    Json::Value response;
    response["type"] = static_cast<int>(Type::self);
    response["action"] = static_cast<int>(Action::roomJoin);
    response["data"] = parse(true);
    connection->send(serializer::json::stringify(response));

    if (_size() > 1) {
        Json::Value message;
        message["type"] = static_cast<int>(Type::other);
        message["action"] = static_cast<int>(Action::roomJoin);
        message["data"] = player->info();
        publish(
                move(serializer::json::stringify(message)),
                player->userId()
        );
    }
}

void Room::unsubscribe(const WebSocketConnectionPtr &connection) {
    _remove(connection);

    const auto &player = connection->getContext<Player>();

    if (connection->connected()) {
        player->reset();
    }

    if (!empty()) {
        Json::Value message;
        message["type"] = static_cast<int>(Type::other);
        message["action"] = static_cast<int>(Action::roomLeave);
        message["data"] = player->userId();
        publish(move(serializer::json::stringify(message)));
    }
}

void Room::unsubscribe(const int64_t &userId) {
    WebSocketConnectionPtr connection;
    {
        shared_lock<shared_mutex> lock(_sharedMutex);
        auto iter = _connectionsMap.find(userId);
        if (iter != _connectionsMap.end()) {
            connection = iter->second;
        } else {
            throw room_exception::PlayerNotFound("Invalid userId");
        }
    }
    unsubscribe(connection);
}

bool Room::empty() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.empty();
}

Json::Value Room::parse(const bool &inner) const {
    Json::Value result;
    result["roomId"] = _roomId;
    result["capacity"] = _capacity.load();
    result["state"] = static_cast<int>(_state.load());

    shared_lock<shared_mutex> lock(_sharedMutex);
    result["info"] = _info;
    result["count"] = _connectionsMap.size();
    if (inner) {
        result["data"] = _data;
        result["players"] = Json::arrayValue;
        for (const auto &[_, connection]: _connectionsMap) {
            result["players"].append(connection->getContext<Player>()->info());
        }
    }

    return result;
}

void Room::publish(string &&message, const int64_t &excludedId) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto &[userId, connection]: _connectionsMap) {
        if (excludedId < 0 || excludedId != userId) {
            connection->send(message);
        }
    }
}

void Room::tell(
        string &&message,
        const int64_t &userId
) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    auto iter = _connectionsMap.find(userId);
    if (iter != _connectionsMap.end()) {
        iter->second->send(message);
        return;
    }
    throw room_exception::PlayerNotFound("Invalid userId");
}

void Room::changeAdmin(
        const WebSocketConnectionPtr &connection,
        const int64_t &userId
) {
    shared_lock<shared_mutex> lock(_sharedMutex);
    auto iter = _connectionsMap.find(userId);
    if (iter != _connectionsMap.end()) {
        iter->second->getContext<Player>()->setRole(Player::Role::super);
        connection->getContext<Player>()->setRole(Player::Role::normal);
        return;
    }
    throw room_exception::PlayerNotFound("Invalid userId");
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
    _state = State::starting;

    Json::Value message;
    message["type"] = static_cast<int>(Type::server);
    message["action"] = static_cast<int>(Action::gameReady);
    publish(serializer::json::stringify(message));

    _startingGame();
}

void Room::cancelStarting() {
    app().getLoop()->invalidateTimer(_timerId);

    _state = State::pending;
}

void Room::checkFinished() {

}

bool Room::_full() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto&[_, connection]: _connectionsMap) {
        if (connection->getContext<Player>()->getType() == Player::Type::gamer) {
            ++counter;
        }
    }
    return counter == _capacity;
}

uint64_t Room::_size() const {
    shared_lock<shared_mutex> lock(_sharedMutex);
    return _connectionsMap.size();
}

uint64_t Room::_count() const {
    uint64_t counter{};
    shared_lock<shared_mutex> lock(_sharedMutex);
    for (const auto&[_, connection]: _connectionsMap) {
        if (connection->getContext<Player>()->getType() == Player::Type::gamer) {
            ++counter;
        }
    }
    return counter;
}

void Room::_insert(const WebSocketConnectionPtr &connection) {
    const auto &player = connection->getContext<Player>();
    if (!player->getJoinedId().empty()) {
        throw room_exception::PlayerOverFlow("Already in a room");
    }
    player->setJoinedId(roomId());
    unique_lock<shared_mutex> lock(_sharedMutex);
    _connectionsMap[player->userId()] = connection;
}

void Room::_remove(const WebSocketConnectionPtr &connection) {
    const auto &player = connection->getContext<Player>();
    if (player->getJoinedId() != roomId()) {
        throw room_exception::PlayerNotFound("Not in this room");
    }
    unique_lock<shared_mutex> lock(_sharedMutex);
    _connectionsMap.erase(player->userId());
}

void Room::_startingGame() {
    app().getLoop()->runAfter(3, [this]() {
        _state = State::started;
        // TODO: Connect to transfer node
        auto client = HttpClient::newHttpClient("http://" + app().getPlugin<AuthMaintainer>()->getReportAddress());
        auto req = HttpRequest::newHttpRequest();
        req->setPath("/tech/api/v2/allocator/transfer");
        req->addHeader("x-credential", app().getPlugin<Authorizer>()->getCredential());
        client->sendRequest(req, [this](ReqResult result, const HttpResponsePtr &responsePtr) {
            if (result == ReqResult::Ok) {
                Json::Value response;
                string parseError = http::toJson(responsePtr, response);
                if (!parseError.empty()) {
                    LOG_WARN << "Invalid response body (" << responsePtr->getStatusCode() << "): \n"
                             << parseError;
                } else if (responsePtr->getStatusCode() != k200OK) {
                    LOG_WARN << "Request failed (" << responsePtr->getStatusCode() << "): \n"
                             << serializer::json::stringify(response);
                } else {
                    Json::Value message;
                    message["type"] = static_cast<int>(Type::server);
                    message["action"] = static_cast<int>(Action::gameStart);
                    message["data"] = response["data"];
                    publish(serializer::json::stringify(message));
                }
            } else {
                LOG_ERROR << "Request failed (" << static_cast<int>(result) << ")";
            }
        }, 5);
    });
}

Room::~Room() {
    for (const auto &[_, connection]: _connectionsMap) {
        if (connection->connected() && connection->hasContext()) {
            connection->getContext<Player>()->reset();
        }
    }
}
