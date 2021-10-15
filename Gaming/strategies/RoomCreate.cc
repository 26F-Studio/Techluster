//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomCreate.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomCreate::RoomCreate() : MessageHandler(toUInt(Action::roomCreate)) {}

Result RoomCreate::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isObject() &&
            request["data"] && request["data"]["capacity"].isUInt64() &&
            request["data"] && request["data"]["info"].isObject() &&
            request["data"] && request["data"]["data"].isObject()
    )) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = "Invalid argument(s)";
        return Result::failed;
    }

    string password;
    if (request["data"] && request["data"]["password"].isString()) {
        password = request["data"]["password"].asString();
    }

    try {
        const auto &roomManager = app().getPlugin<RoomManager>();

        auto roomId = roomManager->createRoom(
                password,
                request["data"]["capacity"].asUInt64(),
                request["data"]["info"],
                request["data"]["data"]
        );

        wsConnPtr->getContext<Player>()->setRole(Player::Role::super);

        roomManager->joinRoom(
                wsConnPtr,
                roomId,
                password
        );
        return Result::silent;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
