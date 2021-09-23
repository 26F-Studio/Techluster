//
// Created by Particle_G on 2021/8/19.
//

#pragma once

#include <plugins/NodeManager.h>
#include <utils/http.h>

namespace tech::services {
    class HeartBeat {
    public:
        HeartBeat();

        Json::Value report(drogon::HttpStatusCode &code, const Json::Value &request);

    private:
        tech::plugins::NodeManager *_nodeManager;
    };
}
