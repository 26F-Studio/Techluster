//
// Created by Parti on 2021/4/9.
//

#pragma once

namespace tech::types {
    enum class Action {
        /// Room actions
        RoomCreate = 1000,
        RoomDataGet,
        RoomDataUpdate,
        RoomInfoGet,
        RoomInfoUpdate,
        RoomJoin,
        RoomKick,
        RoomLeave,
        RoomList,
        RoomPassword,
        RoomRemove,

        /// Player actions
        PlayerConfig = 1100,
        PlayerFinish,
        PlayerGroup,
        PlayerPing,
        PlayerReady,
        PlayerRole,
        PlayerState,
        PlayerType,

        /// Game actions
        GameEnd = 1200,
        GameReady,
        GameSpectate,
        GameStart,
    };
}
