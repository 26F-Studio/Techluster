//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/PlayerState.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

PlayerState::PlayerState() : MessageHandler(toUInt(Action::playerState)) {}

Result PlayerState::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isInt()
    )) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = "Invalid argument(s)";
        return Result::failed;
    }

    const auto &player = wsConnPtr->getContext<Player>();
    const auto &stateNumber = request["data"].asInt();
    Json::Value data;
    data["userId"] = player->userId();
    data["state"] = stateNumber;

    try {
        player->setState(Player::toState(stateNumber));

        app().getPlugin<RoomManager>()->getSharedRoom(
                player->getJoinedId()
        ).room.publish(
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
