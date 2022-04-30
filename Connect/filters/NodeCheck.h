//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks "Node::check" url parameter "address"
 * @param address: in query string
 * @return address: in request attributes
 */

namespace tech::filters {
    class NodeCheck :
            public drogon::HttpFilter<NodeCheck>,
            public helpers::I18nHelper<NodeCheck> {
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