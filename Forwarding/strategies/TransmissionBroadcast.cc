//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/TransmissionManager.h>
#include <strategies/TransmissionBroadcast.h>
#include <structures/Transmitter.h>
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

TransmissionBroadcast::TransmissionBroadcast() : MessageHandlerBase(enum_integer(Action::TransmissionBroadcast)) {}

bool TransmissionBroadcast::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &transmitter = wsConnPtr->getContext<Transmitter>();
    if (!transmitter || transmitter->getRoomId().empty() ||
        transmitter->type != Transmitter::Type::Gamer) {
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

void TransmissionBroadcast::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        app().getPlugin<TransmissionManager>()->transmissionBroadcast(
                _action,
                wsConnPtr,
                move(request.ref().asString())
        );
    }, _action, wsConnPtr);
}
