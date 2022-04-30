//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter trims "User::getData" request body
 * @param paths: Array of Strings
 * @return requestJson: in request attributes
 */
namespace tech::filters {
    class UserGetData :
            public drogon::HttpFilter<UserGetData>,
            public structures::RequestJsonHandler<UserGetData> {
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