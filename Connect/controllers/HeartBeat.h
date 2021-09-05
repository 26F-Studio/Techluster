//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <drogon/HttpController.h>
#include <services/HeartBeat.h>

namespace tech::api::v2 {
    class HeartBeat : public drogon::HttpController<HeartBeat> {
    public:
        METHOD_LIST_BEGIN
            METHOD_ADD(HeartBeat::report, "/report", drogon::Post);
        METHOD_LIST_END

        void report(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    private:
        services::HeartBeat _service;
    };
}

