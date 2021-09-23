//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <services/Monitor.h>

namespace tech::api::v2 {
    class Monitor : public drogon::HttpController<Monitor> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(Monitor::selfInfo, "/self", drogon::Get, drogon::Options, "drogon::LocalHostFilter");
            METHOD_ADD(Monitor::othersInfo, "/{1}", drogon::Get, drogon::Options, "drogon::LocalHostFilter");
        METHOD_LIST_END

        void selfInfo(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

        void othersInfo(
                const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                const std::string &nodeType
        );

    private:
        services::Monitor _service;
    };
}

