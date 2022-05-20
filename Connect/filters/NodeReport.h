//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Node::report" request body
 * @param ip: String
 * @param port: UInt
 * @param taskInterval: Double
 * @param description: String
 * @param info: Object
 * @return requestJson: in request attributes
 */

// TODO: Use requestExceptionHandler
namespace tech::filters {
    class NodeReport :
            public drogon::HttpFilter<NodeReport>,
            public structures::RequestJsonHandler<NodeReport> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}