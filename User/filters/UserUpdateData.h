//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter trims "User::updateData" request body
 * @param data: Array of Objects
 * @param data[*].path: String
 * @param data[*].value: Object
 * @param options: Object of booleans | Optional
 * @return requestJson: in request attributes
 */
namespace tech::filters {
    class UserUpdateData :
            public drogon::HttpFilter<UserUpdateData>,
            public structures::RequestJsonHandler<UserUpdateData> {
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