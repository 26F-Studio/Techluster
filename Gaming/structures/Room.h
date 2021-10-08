//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <drogon/WebSocketConnection.h>
#include <shared_mutex>

namespace tech::structures {
    class Room {
    public:
        enum class State {
            waiting,
            starting,
            started,
        };

        explicit Room(
                std::string roomId,
                const std::string &password,
                const uint64_t &capacity,
                Json::Value info,
                Json::Value data
        );

        void subscribe(drogon::WebSocketConnectionPtr connection);

        void unsubscribe(const drogon::WebSocketConnectionPtr &connection);

        bool isEmpty() const;

        bool isFull() const;

        std::string getRoomId() const;

        uint64_t getCount() const;

        uint64_t getCapacity() const;

        Json::Value getInfo(const std::string &key = "") const;

        void setInfo(const std::string &key, Json::Value value);

        Json::Value getData(const std::string &key = "") const;

        void setData(const std::string &key, Json::Value value);

    private:
        mutable std::shared_mutex _sharedMutex;
        const std::string _roomId, _passwordHash;
        uint64_t _capacity;
        Json::Value _info, _data;
        std::unordered_map<uint64_t, drogon::WebSocketConnectionPtr> _connectionsMap;
    };
}
