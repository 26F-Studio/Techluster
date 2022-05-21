//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/PlayerState.h>
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

PlayerState::PlayerState() : MessageHandlerBase(enum_integer(Action::PlayerState)) {}

bool PlayerState::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty() ||
        player->type == Player::Type::Spectator ||
        player->state != Player::State::Playing) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }

    if (!request.check(JsonValue::String)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void PlayerState::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        wsConnPtr->getContext<Player>()->setCustomState(move(request.ref().asString()));
        app().getPlugin<RoomManager>()->playerState(_action, wsConnPtr);
    }, _action, wsConnPtr);
}
