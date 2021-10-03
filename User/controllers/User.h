//
// Created by particleg on 2021/9/24.
//

#pragma once

#include <drogon/HttpController.h>
#include <services/User.h>

namespace tech::api::v2 {
    class User : public drogon::HttpController<User> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(User::getInfo, "/info", drogon::Get);
            METHOD_ADD(User::updateInfo, "/info", drogon::Put);
            METHOD_ADD(User::getAvatar, "/avatar", drogon::Get);
            METHOD_ADD(User::getData, "/data", drogon::Post, "tech::filters::ValidateField");
            METHOD_ADD(User::updateData, "/data", drogon::Put, "tech::filters::ValidateField");
        METHOD_LIST_END

        void getInfo(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

        void updateInfo(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

        void getAvatar(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

        void getData(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

        void updateData(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback
        );

    private:
        services::User _service;
    };
}