//
// Created by Parti on 2021/4/9.
//

#pragma once

namespace tech::types {
    enum class Action {
        /// Room actions
        roomCreate = 1000,
        roomData,
        roomInfo,
        roomJoin,
        roomKick,
        roomLeave,
        roomList,
        roomPassword,
        roomRemove,

        /// Player actions
        playerConfig = 1100,
        playerFinish,
        playerGroup,
        playerPing,
        playerReady,
        playerRole,
        playerState,
        playerType,

        /// Game actions
        gameEnd = 1200,
        gameReady,
        gameSpectate,
        gameStart,
    };
}
