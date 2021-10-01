//
// Created by particleg on 2021/9/27.
//

#include <filters/ValidateField.h>
#include <structures/DataField.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::filters;
using namespace tech::structures;
using namespace tech::utils;

void ValidateField::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&filterCallback,
        FilterChainCallback &&filterChainCallback
) {
    auto field = req->getParameter("field");
    if (field == "public") {
        req->attributes()->insert("field", DataField::kPublic);
    } else if (field == "protected") {
        req->attributes()->insert("field", DataField::kProtected);
    } else if (field == "private") {
        req->attributes()->insert("field", DataField::kPrivate);
    } else {
        Json::Value response;
        response["type"] = "Failed";
        response["reason"] = "Invalid field";
        http::fromJson(k400BadRequest, response, filterCallback);
        return;
    }
    filterChainCallback();
}
