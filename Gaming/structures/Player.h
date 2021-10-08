//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <shared_mutex>

namespace tech::structures {
    class Player {
    public:
        [[nodiscard]] bool isInRoom() const;

        void setInRoom(const bool &inRoom);

    private:
        mutable std::shared_mutex _sharedMutex;
        std::atomic<bool> _inRoom;
    };
}