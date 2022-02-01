//
// Created by Parti on 2021/2/5.
//

#include <structures/JsonHelper.h>
#include <utils/websocket.h>

using namespace tech::structures;
using namespace tech::utils;
using namespace drogon;
using namespace std;

void websocket::initPing(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &initMessage,
        const chrono::duration<long double> &interval
) {
    wsConnPtr->send(JsonHelper(initMessage).stringify());
    wsConnPtr->setPingMessage("", interval);
}