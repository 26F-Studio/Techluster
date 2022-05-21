//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/NodeMaintainer.h>
#include <plugins/RoomManager.h>
#include <strategies/RoomInfoUpdate.h>
#include <types/Action.h>
#include <types/JsonValue.h>
#include <types/Permission.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::types;

RoomInfoUpdate::RoomInfoUpdate() : MessageHandlerBase(enum_integer(Action::RoomInfoUpdate)) {}

bool RoomInfoUpdate::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    /// @note Return false if the player does not exist or is busy.
    if (!player || player->state != Player::State::Standby) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }
    /// @note Check if accessing current room.
    if (request.check("roomId", JsonValue::String) &&
        request["roomId"].asString() != player->getRoomId()) {
        /// @note Return false if the player is not a global admin.
        const auto info = app().getPlugin<NodeMaintainer>()->getUserInfo(player->userId);
        const auto permission = enum_cast<Permission>(info["permission"].asString()).value();
        if (permission != Permission::Admin) {
            MessageJson message(_action);
            message.setMessageType(MessageType::Failed);
            message.setReason(i18n("noPermission"));
            message.sendTo(wsConnPtr);
            return false;
        }
    } else if (!player->getRoomId().empty()) {
        /// @note Return false if the player is not a room admin.
        if (player->role < Player::Role::Admin) {
            MessageJson message(_action);
            message.setMessageType(MessageType::Failed);
            message.setReason(i18n("noPermission"));
            message.sendTo(wsConnPtr);
            return false;
        }
    } else {
        /// @note Return false if no room is specified.
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("roomNotFound"));
        message.sendTo(wsConnPtr);
        return false;
    }
    /// @note Return false if the data is not an array.
    if (!request.check("data", JsonValue::Array)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void RoomInfoUpdate::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    string roomId = player->getRoomId();
    if (request.check("roomId", JsonValue::String)) {
        roomId = request["roomId"].asString();
    }
    handleExceptions([&]() {
        app().getPlugin<RoomManager>()->roomInfoUpdate(
                _action,
                wsConnPtr,
                roomId,
                request["data"]
        );
    }, _action, wsConnPtr);
}
