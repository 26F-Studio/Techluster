//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <filters/IdentifyUser.h>
#include <helpers/I18nHelper.h>
#include <plugins/ConnectionManager.h>
#include <plugins/HandlerManager.h>
#include <plugins/TransmissionManager.h>

namespace tech::ws::v2 {
    class Forwarding :
            public BaseWebsocket<Forwarding, plugins::HandlerManager>,
            public helpers::I18nHelper<Forwarding> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/tech/ws/v2/forwarding", "tech::filters::IdentifyUser")
        WS_PATH_LIST_END

        Forwarding();

        void handleNewConnection(
                const drogon::HttpRequestPtr &req,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) override;

        void handleConnectionClosed(const drogon::WebSocketConnectionPtr &wsConnPtr) override;

        [[nodiscard]] std::string reason(const std::string &param) const override;

    private:
        plugins::ConnectionManager *_connectionManager;
        plugins::TransmissionManager *_transmissionManager;
    };
}