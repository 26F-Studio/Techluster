//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/PlayerRole.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

PlayerRole::PlayerRole() : MessageHandlerBase(toUInt(Action::playerRole)) {}

Result PlayerRole::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isInt64()
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

    const auto &target = request["data"].asInt64();
    Json::Value data;
    data["userId"] = player->userId();
    data["target"] = target;

    try {
        auto &room = app().getPlugin<RoomManager>()->getSharedRoom(player->getJoinedId()).room;
        room.changeAdmin(wsConnPtr, target);
        room.publish(
                _parseMessage(Type::other, move(data)),
                player->userId()
        );
        response["type"] = static_cast<int>(Type::self);
        return Result::success;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
