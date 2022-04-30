//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/TransmissionManager.h>
#include <structures/ExceptionHandlers.h>

namespace tech::api::v2 {
    class Manage :
            public drogon::HttpController<Manage>,
            public structures::ResponseJsonHandler,
            public helpers::I18nHelper<Manage> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        Manage();

        METHOD_LIST_BEGIN
            METHOD_ADD(
                    Manage::create,
                    "/create",
                    drogon::Post,
                    "tech::filters::VerifyGamingNode",
                    "tech::filters::ManageCreate"
            );
            METHOD_ADD(
                    Manage::remove,
                    "/remove",
                    drogon::Post,
                    "tech::filters::VerifyGamingNode",
                    "tech::filters::ManageRemove"
            );
        METHOD_LIST_END

        void create(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void remove(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::TransmissionManager *_transferManager;
    };
}

