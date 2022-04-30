//
// Created by Particle_G on 2021/3/04.
//

#pragma once

#include <structures/ExceptionHandlers.h>
#include <structures/MessageHandlerBase.h>

namespace tech::strategies {
    class PlayerPing :
            public structures::MessageHandlerBase,
            public structures::MessageJsonHandler<PlayerPing> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        PlayerPing();

        bool filter(const drogon::WebSocketConnectionPtr &wsConnPtr, helpers::RequestJson &request) override;

        void process(const drogon::WebSocketConnectionPtr &wsConnPtr, helpers::RequestJson &request) override;
    };
}
