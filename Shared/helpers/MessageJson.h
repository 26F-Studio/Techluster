//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <drogon/WebSocketConnection.h>
#include <helpers/BasicJson.h>
#include <types/MessageType.h>

namespace tech::helpers {
    class MessageJson : public BasicJson {
    public:
        MessageJson();

        explicit MessageJson(Json::Value json);

        explicit MessageJson(const std::string &raw);

        explicit MessageJson(int action);

        void setMessageType(const types::MessageType &type = types::MessageType::client);

        void setAction(int action);

        void setData(Json::Value data);

        void setData();

        void setReason(const std::exception &e);

        [[maybe_unused]] void setReason(const drogon::orm::DrogonDbException &e);

        void setReason(const std::string &reason);

        void sendTo(const drogon::WebSocketConnectionPtr &connectionPtr) const;

        void closeWith(const drogon::WebSocketConnectionPtr &connectionPtr) const;
    };
}



