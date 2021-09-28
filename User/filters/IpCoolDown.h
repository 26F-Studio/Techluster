//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>

namespace tech::filters {
    class IpCoolDown : public drogon::HttpFilter<IpCoolDown> {
    public:
        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&filterCallback,
                drogon::FilterChainCallback &&filterChainCallback
        ) override;
    };
}