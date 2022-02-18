//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomKick.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomKick::RoomKick() : MessageHandlerBase(toUInt(Action::roomKick)) {}

Result RoomKick::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isUInt64()
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
        const auto &userId = request["data"].asInt64();
        auto &room = app().getPlugin<RoomManager>()->getSharedRoom(
                player->getJoinedId()
        ).room;

        room.tell(_parseMessage(Type::server), userId);
        room.unsubscribe(userId);

        return Result::silent;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
