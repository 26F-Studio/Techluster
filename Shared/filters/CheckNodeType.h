//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks url parameter "nodeType"
 * @param nodeType: in query string
 * @return nodeType: in request attributes
 */

namespace tech::filters {
    class CheckNodeType :
            public drogon::HttpFilter<CheckNodeType, false>,
            public helpers::I18nHelper<CheckNodeType> {
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