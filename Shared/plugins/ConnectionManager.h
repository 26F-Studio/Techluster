//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <drogon/WebSocketConnection.h>
#include <helpers/MessageJson.h>
#include <helpers/I18nHelper.h>
#include <shared_mutex>

namespace tech::plugins {
    class ConnectionManager :
            public drogon::Plugin<ConnectionManager>,
            public helpers::I18nHelper<ConnectionManager> {
    public:
        static constexpr const char *projectName = CMAKE_PROJECT_NAME;

    public:
        ConnectionManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void subscribe(const drogon::WebSocketConnectionPtr &wsConnPtr);

        void unsubscribe(const drogon::WebSocketConnectionPtr &wsConnPtr);

        drogon::WebSocketConnectionPtr getConnPtr(int64_t userId);

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<int64_t, drogon::WebSocketConnectionPtr> _connectionMap;
    };
}

