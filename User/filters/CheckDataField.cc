//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckDataField.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <types/DataField.h>
#include <utils/http.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;
using namespace tech::utils;

CheckDataField::CheckDataField() : I18nHelper(CMAKE_PROJECT_NAME) {}

void CheckDataField::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto dataField = enum_cast<DataField>(req->getParameter("dataField"));
    if (dataField.has_value()) {
        req->attributes()->insert("dataField", dataField.value());
        nextCb();
    } else {
        ResponseJson response;
        response.setResult(ResultCode::notAcceptable);
        response.setMessage(i18n("invalidDataField"));
        http::fromJson(
                k406NotAcceptable,
                response.ref(),
                failedCb
        );
    }
}
