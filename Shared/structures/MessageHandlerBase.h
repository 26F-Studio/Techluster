//
// Created by Particle_G on 2021/2/17.
//

#pragma once

#include <drogon/WebSocketController.h>
#include <helpers/BasicJson.h>

namespace tech::structures {
    enum class Result {
        error,
        failed,
        silent,
        success,
    };

    enum class Type {
        error,
        failed,
        other,
        self,
        server,
    };

    class MessageHandlerBase {
    public:
        explicit MessageHandlerBase(const int &action) : _action(action) {}

        virtual Result fromJson(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) = 0;

        virtual ~MessageHandlerBase() = default;

    protected:
        const int _action;

        [[nodiscard]] std::string _parseMessage(
                const Type &type
        ) const {
            Json::Value message;
            message["type"] = static_cast<int>(type);
            message["action"] = _action;
            return helpers::BasicJson(std::move(message)).stringify();
        }

        [[nodiscard]] std::string _parseMessage(
                const Type &type,
                Json::Value &&data
        ) const {
            Json::Value message;
            message["type"] = static_cast<int>(type);
            message["action"] = _action;
            message["data"] = std::move(data);
            return helpers::BasicJson(std::move(message)).stringify();
        }
    };
}