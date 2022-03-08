//
// Created by ParticleG on 2022/2/9.
//

#pragma once

#include <helpers/BasicJson.h>
#include <types/ResultCode.h>

namespace tech::helpers {
    class ResponseJson : public BasicJson {
    public:
        typedef std::function<void(const drogon::HttpResponsePtr &)> HttpCallback;

        ResponseJson();

        explicit ResponseJson(Json::Value json);

        explicit ResponseJson(const std::string &raw);

        explicit ResponseJson(const drogon::HttpResponsePtr &res);

        void setResultCode(const types::ResultCode &code);

        void setResultCode(const uint32_t &code);

        void setStatusCode(drogon::HttpStatusCode code);

        void setMessage(const std::string &message);

        void setData(Json::Value data);

        void setReason(const std::exception &e);

        [[maybe_unused]] void setReason(const drogon::orm::DrogonDbException &e);

        void setReason(const std::string &reason);

        void httpCallback(const HttpCallback &callback);

        void httpCallback(
                const HttpCallback &callback,
                const std::string &cors
        );

    private:
        drogon::HttpStatusCode _statusCode = drogon::k200OK;
    };
}



