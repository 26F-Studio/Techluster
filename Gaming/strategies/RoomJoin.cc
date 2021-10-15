//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomJoin.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomJoin::RoomJoin() : MessageHandler(toUInt(Action::roomJoin)) {}

Result RoomJoin::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    if (!(
            request.isMember("data") && request["data"].isObject() &&
            request["data"] && request["data"]["roomId"].isString()
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
        app().getPlugin<RoomManager>()->joinRoom(
                wsConnPtr,
                request["data"]["roomId"].asString(),
                password
        );
        return Result::silent;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
