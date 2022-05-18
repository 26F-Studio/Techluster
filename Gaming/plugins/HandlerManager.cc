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
#include <strategies/RoomDataGet.h>
#include <strategies/RoomDataUpdate.h>
#include <strategies/RoomInfoGet.h>
#include <strategies/RoomInfoUpdate.h>
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
    _handlerFactory.registerHandler<PlayerConfig>(enum_integer(Action::PlayerConfig));
    _handlerFactory.registerHandler<PlayerGroup>(enum_integer(Action::PlayerGroup));
    _handlerFactory.registerHandler<PlayerPing>(enum_integer(Action::PlayerPing));
    _handlerFactory.registerHandler<PlayerRole>(enum_integer(Action::PlayerRole));
    _handlerFactory.registerHandler<PlayerState>(enum_integer(Action::PlayerState));
    _handlerFactory.registerHandler<PlayerType>(enum_integer(Action::PlayerType));

    _handlerFactory.registerHandler<RoomCreate>(enum_integer(Action::RoomCreate));
    _handlerFactory.registerHandler<RoomDataGet>(enum_integer(Action::RoomDataGet));
    _handlerFactory.registerHandler<RoomDataUpdate>(enum_integer(Action::RoomDataUpdate));
    _handlerFactory.registerHandler<RoomInfoGet>(enum_integer(Action::RoomInfoGet));
    _handlerFactory.registerHandler<RoomInfoUpdate>(enum_integer(Action::RoomInfoUpdate));
    _handlerFactory.registerHandler<RoomJoin>(enum_integer(Action::RoomJoin));
    _handlerFactory.registerHandler<RoomKick>(enum_integer(Action::RoomKick));
    _handlerFactory.registerHandler<RoomLeave>(enum_integer(Action::RoomLeave));
    _handlerFactory.registerHandler<RoomList>(enum_integer(Action::RoomList));
    _handlerFactory.registerHandler<RoomPassword>(enum_integer(Action::RoomPassword));
    _handlerFactory.registerHandler<RoomRemove>(enum_integer(Action::RoomRemove));

    LOG_INFO << "HandlerManager loaded.";
}

void HandlerManager::shutdown() {
    LOG_INFO << "HandlerManager shutdown.";
}
