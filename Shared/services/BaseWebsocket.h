//
// Created by Parti on 2021/2/23.
//

#pragma once

#include <drogon/WebSocketController.h>
#include <strategies/Action.h>
#include <structures/HandlerManagerBase.h>
#include <utils/websocket.h>

namespace tech::services {
    template<class handlerManagerImpl>
    class BaseWebsocket {
    public:
        BaseWebsocket() : _handlerManager(
                drogon::app().getPlugin<handlerManagerImpl>()
        ) {};

        structures::Result requestHandler(
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

        virtual void establish(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const drogon::AttributesPtr &attributes
        ) = 0;

        virtual void close(
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) = 0;

        virtual ~BaseWebsocket() = default;

    private:
        structures::HandlerManagerBase<handlerManagerImpl> *_handlerManager;
    };
}
