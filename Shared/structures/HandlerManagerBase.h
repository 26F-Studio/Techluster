//
// Created by particleg on 2021/10/10.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/HandlerFactory.h>
#include <structures/MessageHandler.h>

namespace tech::structures {
    template<class handlerManagerImpl>
    class HandlerManagerBase : public drogon::Plugin<handlerManagerImpl> {
    public:
        Result process(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const uint32_t &action,
                const Json::Value &request,
                Json::Value &response,
                drogon::CloseCode &code
        ) {
            return _handlerFactory.getHandler(action).fromJson(
                    wsConnPtr, request, response, code
            );
        }

        virtual ~HandlerManagerBase() = default;

    protected:
        HandlerFactory<MessageHandler> _handlerFactory;
    };
}
