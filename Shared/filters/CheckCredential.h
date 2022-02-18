//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks header "x-credential"
 * @param x-credential: in header
 */
namespace tech::filters {
    class CheckCredential :
            public drogon::HttpFilter<CheckCredential>,
            public helpers::I18nHelper<CheckCredential> {
    public:
        CheckCredential();

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}