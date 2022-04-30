//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/I18nHelper.h>
#include <structures/Transmission.h>

namespace tech::plugins {
    class TransmissionManager :
            public drogon::Plugin<TransmissionManager>,
            public helpers::I18nHelper<TransmissionManager> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void create(
                std::string &&roomId,
                std::set<int64_t> &&playerSet
        );

        void remove(const std::string &roomId);

        void transmissionBroadcast(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&message
        );

        void transmissionJoin(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&roomId
        );

        void transmissionLeave(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        );

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<std::string, structures::Transmission> _transmissionMap;
    };
}

