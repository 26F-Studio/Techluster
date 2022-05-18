//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/I18nHelper.h>
#include <structures/Room.h>
#include <structures/Player.h>
#include <types/Action.h>

namespace tech::plugins {
    class RoomManager :
            public drogon::Plugin<RoomManager>,
            public helpers::I18nHelper<RoomManager> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        RoomManager();

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void playerConfig(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void playerFinish(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                Json::Value &&finishData
        );

        void playerGroup(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void playerReady(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void playerRole(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void playerState(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void playerType(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                structures::Player::Type type
        );

        void roomCreate(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                uint32_t capacity,
                std::string &&password,
                Json::Value roomInfo,
                Json::Value roomData
        );

        void roomDataGet(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &roomId
        );

        void roomDataUpdate(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &roomId,
                const Json::Value &data
        );

        void roomInfoGet(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &roomId
        );

        void roomInfoUpdate(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &roomId,
                const Json::Value &data
        );

        void roomJoin(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&roomId,
                std::string &&password
        );

        void roomKick(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void roomLeave(int action, const drogon::WebSocketConnectionPtr &wsConnPtr);

        void roomList(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&search,
                uint64_t begin,
                uint64_t count
        ) const;

        void roomPassword(
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                std::string &&password
        );

        void roomRemove(const drogon::WebSocketConnectionPtr &wsConnPtr);

    private:
        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<std::string, structures::Room> _roomMap;
    };
}

