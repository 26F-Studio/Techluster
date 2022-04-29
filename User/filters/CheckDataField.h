//
// Created by particleg on 2021/9/27.
//

#pragma once

#include <drogon/HttpFilter.h>
#include <helpers/I18nHelper.h>

/**
 * @brief This filter checks param "dataField" and set attribute "dataField"
 * @param dataField: in query string
 * @return dataField: Enum Class
 */
namespace tech::filters {
    class CheckDataField :
            public drogon::HttpFilter<CheckDataField>,
            public helpers::I18nHelper<CheckDataField> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

        void doFilter(
                const drogon::HttpRequestPtr &req,
                drogon::FilterCallback &&failedCb,
                drogon::FilterChainCallback &&nextCb
        ) override;
    };
}