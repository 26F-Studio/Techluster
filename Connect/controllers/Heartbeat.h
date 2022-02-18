//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <plugins/NodeManager.h>

namespace tech::api::v2 {
    class Heartbeat :
            public drogon::HttpController<Heartbeat>,
            public helpers::I18nHelper<Heartbeat> {
    public:
        Heartbeat();

        METHOD_LIST_BEGIN
            METHOD_ADD(
                    Heartbeat::report,
                    "/report",
                    drogon::Post,
                    "tech::filters::CheckNodeType",
                    "tech::filters::HeartBeatReport"
            );
        METHOD_LIST_END

        void report(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        plugins::NodeManager *_nodeManager;
    };
}

