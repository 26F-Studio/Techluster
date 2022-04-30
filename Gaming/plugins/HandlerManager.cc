//
// Created by Parti on 2021/2/19.
//

#include <magic_enum.hpp>
#include <plugins/HandlerManager.h>
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
#include <strategies/RoomPassword.h>
#include <strategies/RoomRemove.h>
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
    _handlerFactory.registerHandler<PlayerConfig>(enum_integer(Action::playerConfig));
    _handlerFactory.registerHandler<PlayerGroup>(enum_integer(Action::playerGroup));
    _handlerFactory.registerHandler<PlayerPing>(enum_integer(Action::playerPing));
    _handlerFactory.registerHandler<PlayerRole>(enum_integer(Action::playerRole));
    _handlerFactory.registerHandler<PlayerState>(enum_integer(Action::playerState));
    _handlerFactory.registerHandler<PlayerType>(enum_integer(Action::playerType));

    _handlerFactory.registerHandler<RoomCreate>(enum_integer(Action::roomCreate));
    _handlerFactory.registerHandler<RoomData>(enum_integer(Action::roomData));
    _handlerFactory.registerHandler<RoomInfo>(enum_integer(Action::roomInfo));
    _handlerFactory.registerHandler<RoomJoin>(enum_integer(Action::roomJoin));
    _handlerFactory.registerHandler<RoomKick>(enum_integer(Action::roomKick));
    _handlerFactory.registerHandler<RoomLeave>(enum_integer(Action::roomLeave));
    _handlerFactory.registerHandler<RoomList>(enum_integer(Action::roomList));
    _handlerFactory.registerHandler<RoomPassword>(enum_integer(Action::roomPassword));
    _handlerFactory.registerHandler<RoomRemove>(enum_integer(Action::roomRemove));

    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
