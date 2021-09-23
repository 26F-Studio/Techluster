//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/HttpController.h>
#include <json/json.h>

namespace tech::utils::http {
    void fromJson(
            const drogon::HttpStatusCode &code,
            const Json::Value &body,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    void fromJson(
            const drogon::HttpStatusCode &code,
            const Json::Value &body,
            const std::string &cors,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    void fromJson(
            const drogon::HttpStatusCode &code,
            const Json::Value &body,
            const std::vector<drogon::Cookie> &cookies,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    [[maybe_unused]] void fromPlain(
            const drogon::HttpStatusCode &code,
            const std::string &body,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    [[maybe_unused]] void fromHtml(
            const drogon::HttpStatusCode &code,
            const std::string &body,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    void from(
            const drogon::HttpStatusCode &code,
            const drogon::ContentType &type,
            const std::string &body,
            const std::function<void(const drogon::HttpResponsePtr &)> &callback
    );

    std::string toJson(
            const drogon::HttpRequestPtr &req,
            Json::Value &result
    );

    std::string toJson(
            const drogon::HttpResponsePtr &req,
            Json::Value &result
    );
}