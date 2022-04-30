//
// Created by Particle_G on 2021/3/04.
//

#pragma once

#include <structures/ExceptionHandlers.h>
#include <structures/MessageHandlerBase.h>

namespace tech::strategies {
    class RoomCreate :
            public structures::MessageHandlerBase,
            public structures::MessageJsonHandler<RoomCreate> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        RoomCreate();

        bool filter(const drogon::WebSocketConnectionPtr &wsConnPtr, helpers::RequestJson &request) override;

        void process(const drogon::WebSocketConnectionPtr &wsConnPtr, helpers::RequestJson &request) override;
    };
}