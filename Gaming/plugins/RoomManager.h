//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <structures/Room.h>

namespace tech::plugins {
    class RoomManager : public drogon::Plugin<RoomManager> {
    public:
        class SharedRoom {
        public:
            explicit SharedRoom(
                    structures::Room &room,
                    std::shared_lock<std::shared_mutex> &&lock
            );

            structures::Room &room;
        private:
            std::shared_lock<std::shared_mutex> _lock;
        };

        class UniqueRoom {
        public:
            explicit UniqueRoom(
                    structures::Room &room,
                    std::unique_lock<std::shared_mutex> &&lock
            );

            structures::Room &room;
        private:
            std::unique_lock<std::shared_mutex> _lock;
        };

        RoomManager() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        [[nodiscard]] std::string createRoom(
                const std::string &password,
                const uint32_t &capacity,
                const Json::Value &info,
                const Json::Value &data
        );

        void removeRoom(const std::string &roomId);

        SharedRoom getSharedRoom(const std::string &roomId);

        UniqueRoom getUniqueRoom(const std::string &roomId);

        void joinRoom(
                const std::string &roomId,
                const std::string &password,
                const drogon::WebSocketConnectionPtr &connection
        );

        void leaveRoom(
                const drogon::WebSocketConnectionPtr &connection
        );

    private:
        struct RoomWithMutex {
            explicit RoomWithMutex(structures::Room &&room);

            RoomWithMutex(RoomWithMutex &&moved) noexcept;

            structures::Room _room;
            mutable std::unique_ptr<std::shared_mutex> _sharedMutex; // TODO: Try not using unique_ptr
        };

        mutable std::shared_mutex _sharedMutex;
        std::unordered_map<std::string, RoomWithMutex> _roomMap;
    };
}

