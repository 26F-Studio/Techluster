//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <plugins/Configurator.h>
#include <plugins/DataManager.h>
#include <utils/http.h>

namespace tech::services {
    class Auth {
    public:
        Auth();

        Json::Value check(
                drogon::HttpStatusCode &code,
                const std::string &accessToken
        );

        Json::Value refresh(
                drogon::HttpStatusCode &code,
                const std::string &refreshToken
        );

        Json::Value verifyEmail(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value loginMail(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value resetEmail(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value migrateEmail(drogon::HttpStatusCode &code, const Json::Value &data);

    private:
        tech::plugins::Configurator *_configurator;
        tech::plugins::DataManager *_dataManager;

        static std::string _getFileContent(std::string_view path);
    };
}
