//
// Created by Parti on 2021/4/9.
//

#pragma once

#include <structures/Exceptions.h>

namespace tech::strategies {
    enum class Action {
        /// Room actions
        roomCreate = 0,
        roomJoin,
        roomKick,
        roomLeave,
        roomList,
        roomRemove,

        /// Player actions
        playerConfig = 10,
        playerGroup,
        playerRole,
        playerState,
        playerType,

        /// Game actions
        gameEnd = 20,
        gameReady,
        gameStart,
    };

    static constexpr uint32_t toUInt(const Action &action) {
        return static_cast<uint32_t>(action);
    }
}
