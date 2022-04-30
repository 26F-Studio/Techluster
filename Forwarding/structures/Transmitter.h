//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <structures/BasicPlayer.h>
#include <shared_mutex>

namespace tech::structures {
    /**
     * @brief Connection between client and server
     *
     * @param userId: int64_t
     * @param _roomId: string
     */
    class Transmitter : public BasicPlayer {
    public:
        enum class Type {
            gamer,
            spectator,
        };

        explicit Transmitter(int64_t userId);

        Transmitter(Transmitter &&transmitter) noexcept;

        [[nodiscard]] std::string getRoomId() const;

        void setRoomId(const std::string &roomId = {});

        Json::Value info() const;

        void reset();

    public:
        std::atomic<Type> type{Type::spectator};

    private:
        mutable std::shared_mutex _sharedMutex;
        std::string _roomId;
    };
}