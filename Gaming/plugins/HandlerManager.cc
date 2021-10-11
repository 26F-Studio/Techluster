//
// Created by Parti on 2021/2/19.
//

#include <plugins/HandlerManager.h>
#include <strategies/Action.h>

using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

void HandlerManager::initAndStart(const Json::Value &config) {
//    _handlerFactory.registerHandler<GetRoomList>(toUInt(Action::getRoomList));

    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
