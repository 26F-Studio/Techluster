//
// Created by Parti on 2021/4/9.
//

#pragma once

namespace tech::types {
    enum class Action {
        /// Transmission actions
        transmissionBroadcast = 2000,
        transmissionJoin,
        transmissionLeave,
        transmissionRemove,
        transmissionStart,
    };
}
