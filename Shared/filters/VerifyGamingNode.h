//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter sends ip:port to Connect node to check if the server is a valid gaming node.
 * @return id: int64_t
 */

namespace tech::filters {
    class VerifyGamingNode :
            public drogon::HttpFilter<VerifyGamingNode, false>,
            public helpers::I18nHelper<VerifyGamingNode> {
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