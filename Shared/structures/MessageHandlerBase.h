//
// Created by Particle_G on 2021/2/17.
//

#pragma once

#include <drogon/WebSocketController.h>
#include <helpers/RequestJson.h>
#include <types/MessageType.h>

namespace tech::structures {
    class MessageHandlerBase {
    public:
        explicit MessageHandlerBase(int action) : _action(action) {}

        virtual bool filter(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                helpers::RequestJson &request
        ) { return true; }

        virtual void process(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                helpers::RequestJson &request
        ) = 0;

        virtual ~MessageHandlerBase() = default;

    protected:
        const int _action;
    };
}