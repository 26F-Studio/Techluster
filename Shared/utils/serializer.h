//
// Created by Particle_G on 2021/9/4.
//

#pragma once

#include <json/json.h>

namespace tech::utils::serializer {
    namespace json{
        std::string stringify(const Json::Value &value, const std::string &indentation = "");
    }
}