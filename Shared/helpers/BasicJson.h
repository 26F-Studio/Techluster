//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <drogon/drogon.h>

namespace tech::helpers {
    class BasicJson {
    public:
        BasicJson();

        explicit BasicJson(Json::Value json);

        explicit BasicJson(const std::string &raw);

        std::string stringify(const std::string &indentation = "");

        Json::Value &ref();

        [[nodiscard]] Json::Value copy() const;

        void operator()(Json::Value &&another);

        virtual ~BasicJson() = default;

    protected:
        Json::Value _value;
    };
}



