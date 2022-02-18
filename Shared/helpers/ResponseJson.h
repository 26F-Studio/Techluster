//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <helpers/BasicJson.h>
#include <types/ResultCode.h>

namespace tech::helpers {
    class ResponseJson : public BasicJson {
    public:
        ResponseJson() : BasicJson() { setResult(types::ResultCode::completed); }

        explicit ResponseJson(Json::Value json) : BasicJson(std::move(json)) {}

        explicit ResponseJson(const std::string &raw) : BasicJson(raw) {}

        void setResult(const types::ResultCode &code);

        void setResult(const uint32_t &code);

        void setMessage(const std::string &message);

        void setData(Json::Value data);

        void setReason(const std::exception &e);

        void setReason(const drogon::orm::DrogonDbException &e);

        void setReason(const std::string &reason);
    };
}



