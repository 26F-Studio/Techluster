//
// Created by Parti on 2021/4/9.
//

#pragma once

namespace tech::types {
    enum class Action {
        /// Transmission actions
        TransmissionBroadcast = 2000,
        TransmissionJoin,
        TransmissionLeave,
        TransmissionRemove,
        TransmissionStart,
    };
}
