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

        Json::Value refresh(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value verifyEmail(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value loginMail(drogon::HttpStatusCode &code, const Json::Value &data);

        Json::Value loginWeChat(drogon::HttpStatusCode &code, const Json::Value &data);

    private:
        tech::plugins::Configurator *_configurator;
        tech::plugins::DataManager *_dataManager;

        std::string _getFileContent(std::string_view path);
    };
}
