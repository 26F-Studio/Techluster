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
     * @param group: uint64_t (default: 0)
     * @param role: Role (default: normal)
     * @param type: Type (default: spectator)
     * @param state: State (default: standby)
     * @param _roomId: string
     * @param _customState: string
     * @param _config: string
     * @param _pingList: json
     */
    class Player : public BasicPlayer {
    public:
        enum class Role {
            normal = 1,
            admin,
            super,
        };
        enum class State {
            standby,
            ready,
            playing,
            finished,
        };
        enum class Type {
            gamer,
            spectator,
        };

        explicit Player(
                int64_t userId,
                Role role = Role::normal,
                State state = State::standby,
                Type type = Type::spectator
        );

        Player(Player &&player) noexcept;

        [[nodiscard]] std::string getRoomId() const;

        void setRoomId(const std::string &roomId = {});

        [[nodiscard]] std::string getCustomState() const;

        void setCustomState(std::string &&customState);

        [[nodiscard]] std::string getConfig() const;

        void setConfig(std::string &&config);

        [[nodiscard]] Json::Value getPingList() const;

        void setPingList(Json::Value &&pingList);

        [[nodiscard]] Json::Value info() const;

        void reset();

    public:
        std::atomic<uint64_t> group{0};
        std::atomic<Role> role;
        std::atomic<State> state;
        std::atomic<Type> type;

    private:
        mutable std::shared_mutex _sharedMutex;
        std::string _roomId, _customState, _config;
        Json::Value _pingList;
    };
}