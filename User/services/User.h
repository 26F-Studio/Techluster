//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <plugins/Configurator.h>
#include <plugins/DataManager.h>
#include <utils/http.h>

namespace tech::services {
    class User {
    public:
        User();

        Json::Value getInfo(
                drogon::HttpStatusCode &code,
                const std::string &accessToken,
                const int64_t &id
        );

        Json::Value updateInfo(
                drogon::HttpStatusCode &code,
                const std::string &accessToken,
                const Json::Value &data
        );

        Json::Value getAvatar(
                drogon::HttpStatusCode &code,
                const std::string &accessToken,
                const int64_t &id
        );

        Json::Value getData(
                drogon::HttpStatusCode &code,
                const std::string &accessToken,
                const int64_t &id,
                const tech::structures::DataField &field,
                const Json::Value &data
        );

        Json::Value updateData(
                drogon::HttpStatusCode &code,
                const std::string &accessToken,
                const tech::structures::DataField &field,
                const Json::Value &data
        );

    private:
        tech::plugins::DataManager *_dataManager;
    };
}
