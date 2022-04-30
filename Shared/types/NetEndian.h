//
// Created by ParticleG on 2022/3/23.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::types {
    class NetEndian {
    public:
        explicit NetEndian(trantor::InetAddress address);

        trantor::InetAddress address;
        uint64_t netEndian;
    };
}


