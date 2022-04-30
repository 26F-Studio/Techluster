//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks param "userId" and set attribute "userId"
 * @param id: in query string
 * @return userId: Int64
 */

namespace tech::filters {
    class CheckUserId :
            public drogon::HttpFilter<CheckUserId>,
            public helpers::I18nHelper<CheckUserId> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}