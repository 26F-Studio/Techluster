//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/RoomRemove.h>
#include <types/Action.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::types;

RoomRemove::RoomRemove() : MessageHandlerBase(enum_integer(Action::RoomRemove)) {}

bool RoomRemove::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty()) {
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
    return true;
}

void RoomRemove::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        app().getPlugin<RoomManager>()->roomRemove(wsConnPtr);
    }, _action, wsConnPtr);
}
