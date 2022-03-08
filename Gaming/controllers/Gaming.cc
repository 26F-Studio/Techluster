//
// Created by Parti on 2021/3/27.
//

#include <controllers/Gaming.h>
#include <plugins/RoomManager.h>
#include <structures/Player.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;
using namespace tech::ws::v2;
using namespace tech::utils;

void Gaming::handleNewConnection(
        const HttpRequestPtr &req,
        const WebSocketConnectionPtr &wsConnPtr
) {
    auto id = req->getAttributes()->get<int64_t>("id");
    wsConnPtr->setContext(make_shared<structures::Player>(id));

    Json::Value initMessage;
    initMessage["type"] = "Connect";
    websocket::initPing(wsConnPtr, initMessage, chrono::seconds(10));
}

void Gaming::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr) {
    if (wsConnPtr->hasContext()) {
        try {
            app().getPlugin<RoomManager>()->leaveRoom(wsConnPtr);
        } catch (const internal::BaseException &error) {
            LOG_DEBUG << "Closed without unsubscription: " << error.what();
        }
    }
}
