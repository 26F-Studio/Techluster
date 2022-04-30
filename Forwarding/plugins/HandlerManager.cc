//
// Created by Parti on 2021/2/19.
//

#include <magic_enum.hpp>
#include <plugins/HandlerManager.h>
#include <strategies/TransmissionBroadcast.h>
#include <strategies/TransmissionJoin.h>
#include <strategies/TransmissionLeave.h>
#include <types/Action.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

void HandlerManager::initAndStart(const Json::Value &config) {
    _handlerFactory.registerHandler<TransmissionBroadcast>(enum_integer(Action::transmissionBroadcast));
    _handlerFactory.registerHandler<TransmissionJoin>(enum_integer(Action::transmissionJoin));
    _handlerFactory.registerHandler<TransmissionLeave>(enum_integer(Action::transmissionLeave));

    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
