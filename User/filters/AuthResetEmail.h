//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Auth::resetEmail" request body.
 * @param email: String
 * @param code: String
 * @param newPassword: String
 * @return requestJson: in request attributes
 */

namespace tech::filters {
    class AuthResetEmail :
            public drogon::HttpFilter<AuthResetEmail>,
            public structures::RequestJsonHandler {
    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}