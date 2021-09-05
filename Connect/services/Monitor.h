//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <plugins/NodeManager.h>
#include <plugins/Perfmon.h>
#include <utils/http.h>

namespace tech::services {
    class Monitor {
    public:
        Monitor();

        Json::Value selfInfo(drogon::HttpStatusCode &code, const Json::Value &request);

        Json::Value othersInfo(drogon::HttpStatusCode &code, const Json::Value &request);

    private:
        tech::plugins::NodeManager *_nodeManager;
        tech::plugins::Perfmon *_perfmon;
    };
}
