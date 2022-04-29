//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Auth::migrateEmail" request body.
 * @param newEmail: String
 * @param code: String
 * @return requestJson: in request attributes
 */

namespace tech::filters {
    class AuthMigrateEmail :
            public drogon::HttpFilter<AuthMigrateEmail>,
            public structures::RequestJsonHandler<AuthMigrateEmail> {
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