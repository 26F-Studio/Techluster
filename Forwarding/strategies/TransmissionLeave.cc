//
// Created by Particle_G on 2021/3/04.
//

#include <helpers/MessageJson.h>
#include <magic_enum.hpp>
#include <plugins/TransmissionManager.h>
#include <strategies/TransmissionLeave.h>
#include <structures/Transmitter.h>
#include <types/Action.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::types;

TransmissionLeave::TransmissionLeave() : MessageHandlerBase(enum_integer(Action::TransmissionLeave)) {}

bool TransmissionLeave::filter(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    const auto &player = wsConnPtr->getContext<Transmitter>();
    if (!player || player->getRoomId().empty()) {
        MessageJson message(_action);
        message.setMessageType(MessageType::Failed);
        message.setReason(i18n("notAvailable"));
        message.sendTo(wsConnPtr);
        return false;
    }
    return true;
}

void TransmissionLeave::process(const WebSocketConnectionPtr &wsConnPtr, RequestJson &request) {
    handleExceptions([&]() {
        app().getPlugin<TransmissionManager>()->transmissionLeave(_action, wsConnPtr);
    }, _action, wsConnPtr);
}
