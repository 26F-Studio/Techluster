//
// Created by Parti on 2021/2/19.
//

#include <plugins/HandlerManager.h>
#include <strategies/Action.h>
#include <strategies/PlayerConfig.h>
#include <strategies/PlayerGroup.h>
#include <strategies/PlayerPing.h>
#include <strategies/PlayerRole.h>
#include <strategies/PlayerState.h>
#include <strategies/PlayerType.h>
#include <strategies/RoomCreate.h>
#include <strategies/RoomData.h>
#include <strategies/RoomInfo.h>
#include <strategies/RoomJoin.h>
#include <strategies/RoomKick.h>
#include <strategies/RoomLeave.h>
#include <strategies/RoomList.h>
#include <strategies/RoomRemove.h>

using namespace tech::plugins;
using namespace tech::strategies;
using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

void HandlerManager::initAndStart(const Json::Value &config) {
    _handlerFactory.registerHandler<PlayerConfig>(toUInt(Action::playerConfig));
    _handlerFactory.registerHandler<PlayerGroup>(toUInt(Action::playerGroup));
    _handlerFactory.registerHandler<PlayerPing>(toUInt(Action::playerPing));
    _handlerFactory.registerHandler<PlayerRole>(toUInt(Action::playerRole));
    _handlerFactory.registerHandler<PlayerState>(toUInt(Action::playerState));
    _handlerFactory.registerHandler<PlayerType>(toUInt(Action::playerType));

    _handlerFactory.registerHandler<RoomCreate>(toUInt(Action::roomCreate));
    _handlerFactory.registerHandler<RoomData>(toUInt(Action::roomData));
    _handlerFactory.registerHandler<RoomInfo>(toUInt(Action::roomInfo));
    _handlerFactory.registerHandler<RoomJoin>(toUInt(Action::roomJoin));
    _handlerFactory.registerHandler<RoomKick>(toUInt(Action::roomKick));
    _handlerFactory.registerHandler<RoomLeave>(toUInt(Action::roomLeave));
    _handlerFactory.registerHandler<RoomList>(toUInt(Action::roomList));
    _handlerFactory.registerHandler<RoomRemove>(toUInt(Action::roomRemove));

    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
