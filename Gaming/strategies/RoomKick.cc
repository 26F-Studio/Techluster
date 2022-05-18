//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/RoomKick.h>
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

RoomKick::RoomKick() : MessageHandlerBase(enum_integer(Action::RoomKick)) {}

bool RoomKick::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty() ||
        player->state != Player::State::standby) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }

    if (!request.check(JsonValue::Int64)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }

    const auto &targetPlayer = app().getPlugin<ConnectionManager>()->getConnPtr(
            request.ref().asInt64()
    )->getContext<Player>();
    if (!targetPlayer ||
        player->userId == targetPlayer->userId ||
        player->getRoomId() != targetPlayer->getRoomId()) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("invalidTarget"));
        message.sendTo(wsConnPtr);
        return false;
    }
    if (player->role < targetPlayer->role) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("noPermission"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void RoomKick::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        app().getPlugin<RoomManager>()->roomKick(
                _action,
                app().getPlugin<ConnectionManager>()->getConnPtr(request.ref().asInt64())
        );
    }, _action, wsConnPtr);
}
