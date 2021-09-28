//
// Created by particleg on 2021/9/24.
//

#pragma once

#include <drogon/HttpController.h>
#include <services/Auth.h>

namespace tech::api::v2 {
    class Auth : public drogon::HttpController<Auth> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(Auth::refresh, "/refresh", drogon::Get);
            METHOD_ADD(Auth::verifyEmail, "/verify/email", drogon::Post, "tech::filters::EmailCoolDown", "tech::filters::IpCoolDown");
            METHOD_ADD(Auth::loginEmail, "/login/email", drogon::Post);
            METHOD_ADD(Auth::loginWeChat, "/login/wechat", drogon::Post);
        METHOD_LIST_END

        void refresh(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void verifyEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void loginEmail(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void loginWeChat(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        services::Auth _service;
    };
}