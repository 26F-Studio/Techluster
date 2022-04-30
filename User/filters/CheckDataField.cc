//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckDataField.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <types/DataField.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;

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
        response.setStatusCode(k406NotAcceptable);
        response.setResultCode(ResultCode::notAcceptable);
        response.setMessage(i18n("invalidDataField"));
        response.httpCallback(failedCb);
    }
}
