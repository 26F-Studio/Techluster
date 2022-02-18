//
// Created by particleg on 2021/10/10.
//

#include <plugins/RoomManager.h>
#include <services/Gaming.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::utils;

void Gaming::establish(
        const WebSocketConnectionPtr &wsConnPtr,
        const AttributesPtr &attributes
) {
    auto id = attributes->get<int64_t>("id");
    wsConnPtr->setContext(make_shared<structures::Player>(id));

    Json::Value initMessage;
    initMessage["type"] = "Connect";
    websocket::initPing(wsConnPtr, initMessage, chrono::seconds(10));
}

void Gaming::close(const WebSocketConnectionPtr &wsConnPtr) {
    if (wsConnPtr->hasContext()) {
        try {
            app().getPlugin<RoomManager>()->leaveRoom(wsConnPtr);
        } catch (const internal::BaseException &error) {
            LOG_DEBUG << "Closed without unsubscription: " << error.what();
        }
    }
}
