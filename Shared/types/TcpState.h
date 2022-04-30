//
// Created by ParticleG on 2022/3/24.
//

#pragma once

namespace tech::types {
    enum class TcpState {
        closed = 1,
        listen,
        synSent,
        synReceived,
        established,
        finWait1,
        finWait2,
        closeWait,
        lastAck,
        closing,
        timeWait,
        deleteTcb,
        reserved = 100,
    };
}