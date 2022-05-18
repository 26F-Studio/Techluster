//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/RoomInfoGet.h>
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

RoomInfoGet::RoomInfoGet() : MessageHandlerBase(enum_integer(Action::RoomInfoGet)) {}

bool RoomInfoGet::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    /// @note Return false if the player does not exist.
    if (!player) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }
    if (request.check("roomId", JsonValue::String) && player->getRoomId().empty()) {
        /// @note Return false if no room is specified.
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("roomNotFound"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void RoomInfoGet::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    string roomId = player->getRoomId();
    if (request.check("roomId", JsonValue::String)) {
        roomId = request["roomId"].asString();
    }
    handleExceptions([&]() {
        app().getPlugin<RoomManager>()->roomInfoGet(
                _action,
                wsConnPtr,
                roomId
        );
    }, _action, wsConnPtr);
}
