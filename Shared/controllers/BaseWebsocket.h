//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <helpers/BasicJson.h>
#include <magic_enum.hpp>
#include <structures/HandlerManagerBase.h>
#include <structures/MessageHandlerBase.h>
#include <utils/websocket.h>

namespace tech::ws::v2 {
    template<class controllerImpl, class handlerManagerImpl>
    class BaseWebsocket : public drogon::WebSocketController<controllerImpl> {
    public:
        BaseWebsocket() : _handlerManager(
                drogon::app().getPlugin<handlerManagerImpl>()
        ) {};

        void handleNewMessage(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&message,
                const drogon::WebSocketMessageType &type
        ) final {
            using namespace drogon;
            using namespace tech::helpers;
            using namespace tech::structures;
            using namespace tech::utils;

            switch (type) {
                case WebSocketMessageType::Text:
                case WebSocketMessageType::Binary: {
                    Json::Value request = BasicJson(message).stringify(), response;
                    CloseCode code;
                    auto result = requestHandler(wsConnPtr, request, response, code);
                    if (result == Result::success || result == Result::failed) {
                        wsConnPtr->send(BasicJson(response).stringify());
                    } else if (result == Result::error) {
                        wsConnPtr->shutdown(code, BasicJson(response).stringify());
                    }
                    break;
                }
                case WebSocketMessageType::Ping:
                    wsConnPtr->send(message, WebSocketMessageType::Pong);
                    break;
                case WebSocketMessageType::Pong:
                    LOG_WARN << "Message from " << wsConnPtr->peerAddr().toIpPort()
                             << " is (" << magic_enum::enum_name(type) << "):" << message;
                    break;
                case WebSocketMessageType::Close:
                    wsConnPtr->forceClose();
                    break;
                case WebSocketMessageType::Unknown:
                    LOG_WARN << "Message from " << wsConnPtr->peerAddr().toIpPort()
                             << " is Unknown";
                    break;
            }
        }

        virtual ~BaseWebsocket() = default;

    protected:
        virtual structures::Result requestHandler(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) {
            if (!(
                    request.isMember("action") && request["action"].isUInt()
            )) {
                response["type"] = "Failed";
                response["reason"] = "Missing param 'action'";
                return structures::Result::failed;
            }
            return _handlerManager->process(
                    wsConnPtr,
                    request["action"].asUInt(),
                    request,
                    response,
                    code
            );
        }

    private:
        structures::HandlerManagerBase<handlerManagerImpl> *_handlerManager;
    };
}
