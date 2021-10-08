//
// Created by Parti on 2021/2/5.
//

#include <utils/websocket.h>

using namespace tech::utils;
using namespace drogon;
using namespace std;

void websocket::initPing(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &initMessage,
        const chrono::duration<long double> &interval
) {
    wsConnPtr->send(websocket::fromJson(initMessage));
    wsConnPtr->setPingMessage("", interval);
}

void websocket::close(
        const WebSocketConnectionPtr &webSocketConnectionPtr,
        CloseCode _code,
        const string &_reason
) {
    webSocketConnectionPtr->shutdown(_code, _reason);
}
