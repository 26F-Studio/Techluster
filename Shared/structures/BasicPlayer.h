//
// Created by particleg on 2021/10/8.
//

#pragma once

#include <drogon/drogon.h>
#include <structures/Exceptions.h>
#include <shared_mutex>

namespace tech::structures {
    /**
     * @brief Basic connection between client and server
     *
     * @param userId: int64_t
     */
    class BasicPlayer {
    public:
        explicit BasicPlayer(int64_t userId);

        virtual ~BasicPlayer() = default;

    public:
        const int64_t userId;
    };
}