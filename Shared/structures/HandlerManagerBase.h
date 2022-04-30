//
// Created by particleg on 2021/10/10.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/HandlerFactory.h>
#include <structures/MessageHandlerBase.h>

namespace tech::structures {
    template<class handlerManagerImpl>
    class HandlerManagerBase : public drogon::Plugin<handlerManagerImpl> {
    public:
        void process(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                helpers::RequestJson &request
        ) {
            auto &handler = _handlerFactory.getHandler(action);
            if (handler.filter(wsConnPtr, request)) {
                handler.process(wsConnPtr, request);
            }
        }

        virtual ~HandlerManagerBase() = default;

    protected:
        HandlerFactory<MessageHandlerBase> _handlerFactory;
    };
}
