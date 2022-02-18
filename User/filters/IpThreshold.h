//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks request frequency from specific ip address
 */
namespace tech::filters {
    class IpThreshold :
            public drogon::HttpFilter<IpThreshold>,
            public helpers::I18nHelper<IpThreshold> {
    public:
        IpThreshold();

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}