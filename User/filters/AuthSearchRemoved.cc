//
// Created by particleg on 2021/9/27.
//

#include <filters/AuthSearchRemoved.h>
#include <helpers/RequestJson.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;

void AuthSearchRemoved::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.trim("id", JsonValue::Int64);
        request.trim("email", JsonValue::String);
        request.trim("username", JsonValue::String);
        request.trim("clan", JsonValue::String);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
