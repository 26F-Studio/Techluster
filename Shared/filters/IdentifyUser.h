//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter sends header "x-access-token" to User node and set attribute "id"
 * @param x-access-token: in header
 * @return id: int64_t
 */

namespace tech::filters {
    class IdentifyUser :
            public drogon::HttpFilter<IdentifyUser, false>,
            public helpers::I18nHelper<IdentifyUser> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        IdentifyUser();

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}