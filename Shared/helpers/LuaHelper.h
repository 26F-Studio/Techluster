//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::helpers {
    class LuaHelper {
    public:
        explicit LuaHelper(const std::string &filename);

    protected:
        Json::Value _value;
    };
}



