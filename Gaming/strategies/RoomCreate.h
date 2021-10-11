//
// Created by Particle_G on 2021/3/04.
//

#pragma once

#include <structures/MessageHandler.h>

namespace tech::strategies {
    class RoomCreate : public structures::MessageHandler {
    public:
        RoomCreate();

        structures::Result fromJson(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) override;
    };
}
