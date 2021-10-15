//
// Created by Particle_G on 2021/3/04.
//

#include <plugins/RoomManager.h>
#include <strategies/Action.h>
#include <strategies/RoomList.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;

RoomList::RoomList() : MessageHandler(toUInt(Action::roomList)) {}

Result RoomList::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response,
        CloseCode &code
) {
    string search;
    uint64_t begin = 0, count = 10;
    if (request.isMember("data") && request["data"].isObject()) {
        if (request["data"].isMember("search") && request["data"]["search"].isString()) {
            search = request["data"]["search"].asString();
        }

        if (request["data"].isMember("begin") && request["data"]["begin"].isUInt64()) {
            begin = request["data"]["begin"].asUInt64();
        }

        if (request["data"].isMember("count") && request["data"]["count"].isUInt64()) {
            count = request["data"]["count"].asUInt64();
        }
    }

    try {
        response["type"] = static_cast<int>(Type::self);
        response["data"] = app().getPlugin<RoomManager>()->roomList(
                search,
                begin,
                count
        );
        return Result::success;
    } catch (const exception &error) {
        response["type"] = static_cast<int>(Type::failed);
        response["reason"] = error.what();
        return Result::failed;
    }
}
