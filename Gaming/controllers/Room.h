//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <services/Room.h>

namespace tech::socket::v2 {
    class Room : public BaseWebsocket<Room, tech::services::Room> {
    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/tech/socket/v2/room", "tech::filters::CheckAccessToken")
        WS_PATH_LIST_END

        Room();
    };
}