//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/RoomManager.h>
#include <strategies/PlayerFinish.h>
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

PlayerFinish::PlayerFinish() : MessageHandlerBase(enum_integer(Action::playerFinish)) {}

bool PlayerFinish::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Player>();
    if (!player || player->getRoomId().empty() ||
        player->type == Player::Type::spectator ||
        player->state != Player::State::playing) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }

    if (!request.check(JsonValue::Object)) {
        MessageJson message(_action);
        message.setMessageType(MessageType::failed);
        message.setReason(i18n("invalidArguments"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void PlayerFinish::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        wsConnPtr->getContext<Player>()->state = Player::State::finished;
        app().getPlugin<RoomManager>()->playerFinish(_action, wsConnPtr, move(request.ref()));
    }, _action, wsConnPtr);
}
