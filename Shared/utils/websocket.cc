//
// Created by Parti on 2021/2/5.
//

#include <utils/serializer.h>
#include <utils/websocket.h>

using namespace tech::utils;
using namespace drogon;
using namespace std;

void websocket::initPing(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &initMessage,
        const chrono::duration<long double> &interval
) {
    wsConnPtr->send(serializer::json::stringify(initMessage));
    wsConnPtr->setPingMessage("", interval);
}