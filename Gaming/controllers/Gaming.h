//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <filters/IdentifyUser.h>
#include <plugins/HandlerManager.h>

namespace tech::ws::v2 {
    class Gaming : public BaseWebsocket<Gaming, plugins::HandlerManager> {
    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/tech/ws/v2/gaming", "tech::filters::IdentifyUser")
        WS_PATH_LIST_END

        void handleNewConnection(
                const drogon::HttpRequestPtr &req,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) override;

        void handleConnectionClosed(const drogon::WebSocketConnectionPtr &wsConnPtr) override;
    };
}