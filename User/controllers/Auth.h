//
// Created by particleg on 2021/9/24.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/DataManager.h>
#include <structures/ExceptionHandlers.h>

namespace tech::api::v2 {
    class Auth :
            public drogon::HttpController<Auth>,
            public structures::ResponseJsonHandler,
            public helpers::I18nHelper<Auth> {
    public:
        Auth();

        METHOD_LIST_BEGIN
            METHOD_ADD(Auth::check, "/check", drogon::Get, "tech::filters::CheckAccessToken");
            METHOD_ADD(Auth::refresh, "/refresh", drogon::Get, "tech::filters::CheckRefreshToken");
            METHOD_ADD(
                    Auth::verifyEmail,
                    "/verify/email",
                    drogon::Post,
                    "tech::filters::AuthVerifyEmail",
                    "tech::filters::EmailThreshold",
                    "tech::filters::IpThreshold"
            );
            METHOD_ADD(Auth::loginEmail, "/login/email", drogon::Post, "tech::filters::AuthLoginEmail");
            METHOD_ADD(Auth::resetEmail, "/reset/email", drogon::Put, "tech::filters::AuthResetEmail");
            METHOD_ADD(
                    Auth::migrateEmail,
                    "/migrate/email",
                    drogon::Put,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::AuthMigrateEmail");
        METHOD_LIST_END

        void check(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void refresh(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void verifyEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void loginEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void resetEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void migrateEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::DataManager *_dataManager;
    };
}