//
// Created by particleg on 2021/9/27.
//

#include <filters/ManageCreate.h>
#include <helpers/RequestJson.h>
#include <structures/Exceptions.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;

void ManageCreate::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    handleExceptions([&]() {
        auto request = RequestJson(req);
        request.require("roomId", JsonValue::String);
        request.require("players", JsonValue::Array);
        req->attributes()->insert("requestJson", request);
        nextCb();
    }, failedCb);
}
