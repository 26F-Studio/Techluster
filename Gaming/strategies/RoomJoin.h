//
// Created by Particle_G on 2021/3/04.
//

#pragma once

#include <structures/MessageHandlerBase.h>

namespace tech::strategies {
    class RoomJoin : public structures::MessageHandlerBase {
    public:
        RoomJoin();

        structures::Result fromJson(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) override;
    };
}
