//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <helpers/I18nHelper.h>
#include <plugins/NodeManager.h>
#include <plugins/Perfmon.h>
#include <structures/ExceptionHandlers.h>

namespace tech::api::v2 {
    class Monitor :
            public drogon::HttpController<Monitor>,
            public structures::ResponseJsonHandler,
            public helpers::I18nHelper<Monitor> {
    public:
        Monitor();

        METHOD_LIST_BEGIN
            METHOD_ADD(
                    Monitor::getInfo,
                    "",
                    drogon::Get,
                    drogon::Options,
                    "drogon::LocalHostFilter",
                    "tech::filters::CheckNodeType"
            );
        METHOD_LIST_END

        void getInfo(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::NodeManager *_nodeManager;
        plugins::Perfmon *_perfmon;
    };
}

