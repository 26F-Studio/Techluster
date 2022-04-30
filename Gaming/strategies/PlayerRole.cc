//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/PlayerRole.h>
#include <structures/Player.h>
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

PlayerRole::PlayerRole() : MessageHandlerBase(enum_integer(Action::playerRole)) {}

bool PlayerRole::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty() ||
        player->state != Player::State::standby) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }

    if (!request.check("targetUserId", JsonValue::Int64) ||
        !request.check("role", JsonValue::String)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    auto castedRole = enum_cast<Player::Role>(request["role"].asString());
    if (!castedRole.has_value()) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidRole"));
        message.sendTo(wsConnPtr);
        return false;
    }
    if (player->role < castedRole.value()) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("noPermission"));
        message.sendTo(wsConnPtr);
        return false;
    }

    const auto &targetPlayer = app().getPlugin<ConnectionManager>()->getConnPtr(
            request["targetUserId"].asInt64()
    )->getContext<Player>();
    if (!targetPlayer ||
        player->userId == targetPlayer->userId ||
        player->getRoomId() != targetPlayer->getRoomId()) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidTarget"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void PlayerRole::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        auto roomManager = app().getPlugin<RoomManager>();
        auto targetRole = enum_cast<Player::Role>(request["role"].asString()).value();
        const auto &targetConnPtr = app().getPlugin<ConnectionManager>()->getConnPtr(
                request["targetUserId"].asInt64()
        );
        const auto &player = wsConnPtr->getContext<Player>();
        const auto &targetPlayer = targetConnPtr->getContext<Player>();

        if (player->role == targetRole) {
            player->role = targetPlayer->role.load();
            targetPlayer->role = targetRole;
            roomManager->playerRole(_action, wsConnPtr);
            roomManager->playerRole(_action, targetConnPtr);
        } else {
            targetPlayer->role = targetRole;
            roomManager->playerRole(_action, wsConnPtr);
        }
    }, _action, wsConnPtr);
}
