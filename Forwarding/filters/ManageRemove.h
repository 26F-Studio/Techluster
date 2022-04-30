//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <structures/ExceptionHandlers.h>

/**
 * @brief This filter checks "Manage::remove" request body.
 * @param roomId: String
 * @return requestJson: in request attributes
 */

// TODO: Use requestExceptionHandler
namespace tech::filters {
    class ManageRemove :
            public drogon::HttpFilter<ManageRemove>,
            public structures::RequestJsonHandler<ManageRemove> {
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