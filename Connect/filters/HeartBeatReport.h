//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Heartbeat::report" request body.
 * @param ip: String
 * @param port: UInt
 * @param taskInterval: Double
 * @param description: String
 * @param info: Object
 * @return requestJson: in request attributes
 */

// TODO: Use requestExceptionHandler
namespace tech::filters {
    class HeartBeatReport :
            public drogon::HttpFilter<HeartBeatReport>,
            public structures::RequestJsonHandler {
    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}