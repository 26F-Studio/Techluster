//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <controllers/BaseWebsocket.h>
#include <filters/IdentifyUser.h>
#include <services/Gaming.h>

namespace tech::socket::v2 {
    class Gaming : public BaseWebsocket<Gaming, tech::services::Gaming> {
    public:
        WS_PATH_LIST_BEGIN
            WS_PATH_ADD("/tech/ws/v2/gaming", "tech::filters::CheckAccessToken")
        WS_PATH_LIST_END

        Gaming();
    };
}