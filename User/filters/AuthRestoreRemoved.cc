//
// Created by particleg on 2021/9/27.
//

#include <filters/AuthRestoreRemoved.h>
#include <helpers/RequestJson.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;

void AuthRestoreRemoved::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.require("email", JsonValue::String);
        request.require("code", JsonValue::String);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
