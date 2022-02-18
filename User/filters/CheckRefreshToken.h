//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks header "x-refresh-token" and set attribute "refreshToken"
 * @param x-access-token: in header
 * @return refreshToken: String
 */

namespace tech::filters {
    class CheckRefreshToken :
            public drogon::HttpFilter<CheckRefreshToken>,
            public helpers::I18nHelper<CheckRefreshToken> {
    public:
        CheckRefreshToken();

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}