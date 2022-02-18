//
// Created by particleg on 2021/9/27.
//

#include <filters/CheckNodeType.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <types/NodeType.h>
#include <utils/http.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;
using namespace tech::utils;

CheckNodeType::CheckNodeType() : I18nHelper(CMAKE_PROJECT_NAME) {}

void CheckNodeType::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    auto type = enum_cast<NodeType>(req->getParameter("nodeType"));
    if (type.has_value()) {
        req->attributes()->insert("nodeType", type.value());
        nextCb();
    } else {
        ResponseJson response;
        response.setResult(ResultCode::notAcceptable);
        response.setMessage(i18n("invalidNodeType"));
        http::fromJson(
                k406NotAcceptable,
                response.ref(),
                failedCb
        );
    }
}
