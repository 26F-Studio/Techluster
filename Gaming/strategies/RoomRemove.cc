//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomRemove.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomRemove::RoomRemove() : MessageHandler(toUInt(Action::roomRemove)) {}

Result RoomRemove::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response, CloseCode &code
) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (player->getRole() != Player::Role::super) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = "You are not the superuser";
        return Result::failed;
    }

    try {
        const auto &roomManager = app().getPlugin<RoomManager>();
        roomManager->getSharedRoom(
                player->getJoinedId()
        ).room.publish(_parseMessage(Type::server));
        roomManager->removeRoom(player->getJoinedId());

        return Result::silent;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
