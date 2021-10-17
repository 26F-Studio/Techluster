//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomData.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomData::RoomData() : MessageHandler(toUInt(Action::roomData)) {}

Result RoomData::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isObject()
    )) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = "Invalid argument(s)";
        return Result::failed;
    }

    const auto &player = wsConnPtr->getContext<Player>();
    if (player->getRole() != Player::Role::super) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = "You are not the superuser";
        return Result::failed;
    }

    try {
        auto data = request["data"];
        auto sharedRoom = app().getPlugin<RoomManager>()->getSharedRoom(player->getJoinedId());
        auto &room = sharedRoom.room;
        room.setData(data);
        room.publish(_parseMessage(Type::server, move(data)));
        return Result::silent;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
