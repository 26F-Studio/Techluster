//
// Created by ParticleG on 2022/3/24.
//

#pragma once

namespace tech::types {
    enum class TcpState {
        Closed = 1,
        Listen,
        SynSent,
        SynReceived,
        established,
        FinWait1,
        FinWait2,
        CloseWait,
        LastAck,
        Closing,
        TimeWait,
        DeleteTcb,
        Reserved = 100,
    };
}