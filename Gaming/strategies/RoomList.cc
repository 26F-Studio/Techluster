//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/RoomList.h>
#include <types/Action.h>
#include <types/JsonValue.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::types;

RoomList::RoomList() : MessageHandlerBase(enum_integer(Action::RoomList)) {}

bool RoomList::filter(
        const WebSocketConnectionPtr &wsConnPtr,
        RequestJson &request
) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void RoomList::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        string search;
        uint64_t begin = 0, count = 10;
        if (request.check("search", JsonValue::String)) {
            search = move(request["search"].asString());
        }

        if (request.check("begin", JsonValue::Uint64)) {
            begin = request["begin"].asUInt64();
        }

        if (request.check("count", JsonValue::Uint64)) {
            count = request["count"].asUInt64();
        }

        app().getPlugin<RoomManager>()->roomList(
                _action,
                wsConnPtr,
                move(search),
                begin,
                count
        );
    }, _action, wsConnPtr);
}
