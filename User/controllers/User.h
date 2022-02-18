//
// Created by particleg on 2021/9/24.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/DataManager.h>
#include <structures/ExceptionHandlers.h>

namespace tech::api::v2 {
    class User :
            public drogon::HttpController<User>,
            public structures::ResponseJsonHandler,
            public helpers::I18nHelper<User> {
    public:
        User();

        METHOD_LIST_BEGIN
            METHOD_ADD(
                    User::getInfo,
                    "/info",
                    drogon::Get,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::CheckUserId"
            );
            METHOD_ADD(
                    User::updateInfo,
                    "/info",
                    drogon::Put,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::UserUpdateInfo"
            );
            METHOD_ADD(
                    User::getAvatar,
                    "/avatar",
                    drogon::Get,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::CheckUserId");
            METHOD_ADD(
                    User::getData,
                    "/data",
                    drogon::Post,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::CheckUserId",
                    "tech::filters::CheckDataField",
                    "tech::filters::UserGetData"
            );
            METHOD_ADD(
                    User::updateData,
                    "/data",
                    drogon::Put,
                    "tech::filters::CheckAccessToken",
                    "tech::filters::CheckDataField",
                    "tech::filters::UserUpdateData"
            );
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
        tech::plugins::DataManager *_dataManager;
    };
}