//
// Created by particleg on 2021/10/8.
//

#include <structures/Player.h>
#include <structures/Room.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Room::Room(
        string roomId,
        const string &password,
        const uint64_t &capacity,
        Json::Value info,
        Json::Value data
) : _roomId(move(roomId)),
    _passwordHash(crypto::blake2b(password)),
    _capacity(capacity),
    _info(move(info)),
    _data(move(data)) {}

inline void Room::subscribe(drogon::WebSocketConnectionPtr connection) {
    auto player = connection->getContext<Player>();
    if (!player->getRid().empty()) {
        throw length_error("Can only subscribe one room");
    }
    if (isFull()) {
        throw range_error("Room is full");
    }

    unique_lock<shared_mutex> lock(_sharedMutex);
    if (get<string>(player->getRid()) == _rid) {
        throw overflow_error("Room already subscribed");
    }
    auto sid = _loopCycleID();
    player->setSid(_rid, sid);
    _connectionsMap[sid] = move(connection);
    misc::logger(typeid(*this).name(), "Subscribe: (" + _rid + ") " + to_string(sid));
}

inline void Room::unsubscribe(const WebSocketConnectionPtr &connection) {

}

inline bool Room::isEmpty() const {
    return false;
}

inline bool Room::isFull() const {
    return false;
}

inline string Room::getRoomId() const {
    return std::string();
}

uint64_t Room::getCount() const {
    return 0;
}
