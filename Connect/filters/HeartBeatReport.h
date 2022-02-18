//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks "Heartbeat::report" request body.
 * @param ip: String
 * @param port: UInt
 * @param taskInterval: Double
 * @param description: String
 * @param info: Object
 * @return requestJson: in request attributes
 */

namespace tech::filters {
    class HeartBeatReport :
            public drogon::HttpFilter<HeartBeatReport>,
            public helpers::I18nHelper<HeartBeatReport> {
    public:
        HeartBeatReport();

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}