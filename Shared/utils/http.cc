//
// Created by Parti on 2021/2/4.
//

#include <utils/http.h>

using namespace tech::utils;
using namespace drogon;
using namespace std;

void http::fromJson(
        const HttpStatusCode &code,
        const Json::Value &body,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpJsonResponse = HttpResponse::newHttpJsonResponse(body);
    httpJsonResponse->setStatusCode(code);
    callback(httpJsonResponse);
}

void http::fromJson(
        const HttpStatusCode &code,
        const Json::Value &body,
        const string &cors,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpJsonResponse = HttpResponse::newHttpJsonResponse(body);
    httpJsonResponse->setStatusCode(code);
    httpJsonResponse->addHeader("Access-Control-Allow-Origin", cors);
    callback(httpJsonResponse);
}

void http::fromJson(
        const HttpStatusCode &code,
        const Json::Value &body,
        const vector<Cookie> &cookies,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpJsonResponse = HttpResponse::newHttpJsonResponse(body);
    httpJsonResponse->setStatusCode(code);
    for (const auto &cookie: cookies) {
        httpJsonResponse->addCookie(cookie);
    }
    callback(httpJsonResponse);
}

[[maybe_unused]] void http::fromPlain(
        const HttpStatusCode &code, const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) { from(code, CT_TEXT_PLAIN, body, callback); }

[[maybe_unused]] void http::fromHtml(
        const HttpStatusCode &code,
        const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) { from(code, CT_TEXT_HTML, body, callback); }

void http::from(
        const HttpStatusCode &code,
        const ContentType &type,
        const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpResponse = HttpResponse::newHttpResponse();
    httpResponse->setStatusCode(code);
    httpResponse->setContentTypeCode(type);
    httpResponse->setBody(body);
    callback(httpResponse);
}

string http::toJson(
        const HttpRequestPtr &req,
        Json::Value &result
) {
    auto object = req->getJsonObject();
    if (object) {
        result = *object;
        return req->getJsonError();
    }
    return "Invalid Json";
}

string http::toJson(
        const HttpResponsePtr &req,
        Json::Value &result
) {
    auto object = req->getJsonObject();
    if (object) {
        result = *object;
        return req->getJsonError();
    }
    return "Invalid Json";
}