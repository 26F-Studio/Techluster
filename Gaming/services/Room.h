//
// Created by particleg on 2021/10/10.
//

#pragma once

#include <plugins/HandlerManager.h>
#include <services/BaseWebsocket.h>

namespace tech::services {
    class Room : public BaseWebsocket<plugins::HandlerManager> {
    public:
        void establish(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const drogon::AttributesPtr &attributes
        ) override;

        void close(
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) override;
    };
}