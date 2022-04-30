//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <filters/IdentifyUser.h>
#include <helpers/I18nHelper.h>
#include <plugins/ConnectionManager.h>
#include <plugins/HandlerManager.h>
#include <plugins/RoomManager.h>

namespace tech::ws::v2 {
    class Gaming :
            public BaseWebsocket<Gaming, plugins::HandlerManager>,
            public helpers::I18nHelper<Gaming> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/tech/ws/v2/gaming", "tech::filters::IdentifyUser")
        WS_PATH_LIST_END

        Gaming();

        void handleNewConnection(
                const drogon::HttpRequestPtr &req,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) override;

        void handleConnectionClosed(const drogon::WebSocketConnectionPtr &wsConnPtr) override;

        [[nodiscard]] std::string reason(const std::string &param) const override;

    private:
        plugins::ConnectionManager *_connectionManager;
        plugins::RoomManager *_roomManager;
    };
}