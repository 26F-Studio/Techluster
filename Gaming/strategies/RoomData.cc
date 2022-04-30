//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/RoomData.h>
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

RoomData::RoomData() : MessageHandlerBase(enum_integer(Action::roomData)) {}

bool RoomData::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty() ||
        player->state != Player::State::standby) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }
    if (player->role < Player::Role::admin) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("noPermission"));
        message.sendTo(wsConnPtr);
        return false;
    }

    if (!request.check(JsonValue::Array)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void RoomData::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        app().getPlugin<RoomManager>()->roomData(
                _action,
                wsConnPtr,
                request
        );
    }, _action, wsConnPtr);
}