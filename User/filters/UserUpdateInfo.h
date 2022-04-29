//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter trims "User::updateInfo" request body
 * @param password: String
 * @param username: String
 * @param motto: String
 * @param region: Int64
 * @param avatar: String
 * @param avatar_frame: Int64
 * @param clan: String
 * @return requestJson: in request attributes
 */
namespace tech::filters {
    class UserUpdateInfo :
            public drogon::HttpFilter<UserUpdateInfo>,
            public structures::RequestJsonHandler<UserUpdateInfo> {
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