//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Auth::deactivateEmail" request body.
 * @param code: String
 * @return requestJson: in request attributes
 */

namespace tech::filters {
    class AuthDeactivateEmail :
            public drogon::HttpFilter<AuthDeactivateEmail>,
            public structures::RequestJsonHandler {
    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}