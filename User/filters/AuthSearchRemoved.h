//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter trims "Auth::searchRemoved" request body
 * @param id: Int64
 * @param email: String
 * @param username: String
 * @param clan: String
 * @return requestJson: in request attributes
 */
namespace tech::filters {
    class AuthSearchRemoved :
            public drogon::HttpFilter<AuthSearchRemoved>,
            public structures::RequestJsonHandler<AuthSearchRemoved> {
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