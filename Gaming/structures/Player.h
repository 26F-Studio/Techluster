//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <structures/Exceptions.h>
#include <shared_mutex>

namespace tech::structures {
    class Player {
    public:
        enum class Role {
            normal,
            super,
        };

        enum class Type {
            gamer,
            spectator,
        };

        enum class State {
            dead,
            finish,
            playing,
            ready,
            standby,
        };

        static constexpr Type toType(const int &type) {
            switch (type) {
                case 0:
                    return Type::gamer;
                case 1:
                    return Type::spectator;
                default:
                    throw action_exception::InvalidArgument("Invalid type number");
            }
        }

        static constexpr State toState(const int &state) {
            switch (state) {
                case 0:
                    return State::dead;
                case 1:
                    return State::finish;
                case 2:
                    return State::playing;
                case 3:
                    return State::ready;
                case 4:
                    return State::standby;
                default:
                    throw action_exception::InvalidArgument("Invalid state number");
            }
        }

        explicit Player(const int64_t &userId);

        [[nodiscard]] const int64_t &userId() const;

        [[nodiscard]] uint32_t getGroup() const;

        void setGroup(const uint32_t &group);

        [[nodiscard]] Role getRole() const;

        void setRole(const Role &role);

        [[nodiscard]] Type getType() const;

        void setType(const Type &type);

        [[nodiscard]] State getState() const;

        void setState(const State &state);

        [[nodiscard]] std::string getConfig() const;

        void setConfig(std::string &&config);

        [[nodiscard]] std::string getJoinedId() const;

        void setJoinedId(const std::string &joinedId = "");

        [[nodiscard]] Json::Value info() const;

        void reset();

    private:
        mutable std::shared_mutex _sharedMutex;
        const int64_t _userId;
        std::atomic<uint32_t> _group;
        std::atomic<Role> _role;
        std::atomic<Type> _type;
        std::atomic<State> _state;
        std::string _config, _joinedId;
    };
}