//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <helpers/BasicJson.h>
#include <structures/MessageHandlerBase.h>
#include <utils/websocket.h>

namespace tech::socket::v2 {
    template<class controllerImpl, class serviceImpl>
    class BaseWebsocket : public drogon::WebSocketController<controllerImpl> {
    public:
        virtual void handleNewMessage(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&message,
                const drogon::WebSocketMessageType &type
        ) {
            using namespace drogon;
            using namespace tech::helpers;
            using namespace tech::structures;
            using namespace tech::utils;

            if (type == WebSocketMessageType::Ping) {
                wsConnPtr->send(message, WebSocketMessageType::Pong);
            } else if (type == WebSocketMessageType::Text || type == WebSocketMessageType::Binary) {
                Json::Value request = BasicJson(message).stringify(), response;
                CloseCode code;
                auto result = _service.requestHandler(wsConnPtr, request, response, code);
                if (result == Result::success || result == Result::failed) {
                    wsConnPtr->send(BasicJson(response).stringify());
                } else if (result == Result::error) {
                    wsConnPtr->shutdown(code, BasicJson(response).stringify());
                }
            } else if (type == WebSocketMessageType::Close) {
                wsConnPtr->forceClose();
            } else if (type == WebSocketMessageType::Unknown) {
                LOG_WARN << "Message from " << wsConnPtr->peerAddr().toIpPort() << " is Unknown";
            } else if (type != WebSocketMessageType::Pong) {
                LOG_WARN << "Message from " << wsConnPtr->peerAddr().toIpPort() << " is (" << static_cast<int>(type) << "):" << message;
            }
        }

        virtual void handleNewConnection(const drogon::HttpRequestPtr &req, const drogon::WebSocketConnectionPtr &wsConnPtr) {
            _service.establish(wsConnPtr, req->getAttributes());
        }

        virtual void handleConnectionClosed(const drogon::WebSocketConnectionPtr &wsConnPtr) {
            _service.close(wsConnPtr);
        }

        virtual ~BaseWebsocket() = default;

    protected:
        serviceImpl _service{};
    };
}
