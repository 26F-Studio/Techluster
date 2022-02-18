//
// Created by particleg on 2021/9/27.
//

#include <filters/UserUpdateData.h>
#include <helpers/RequestJson.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::types;

void UserUpdateData::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.require("data", JsonValue::Array);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
