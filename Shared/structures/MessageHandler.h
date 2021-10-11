//
// Created by Particle_G on 2021/2/17.
//

#pragma once

#include <drogon/WebSocketController.h>

namespace tech::structures {
    enum class Result {
        error,
        failed,
        silent,
        success,
    };

    enum class Origin {
        other,
        self,
        server,
    };

    class MessageHandler {
    public:
        virtual Result fromJson(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) = 0;

        virtual ~MessageHandler() = default;
    };
}