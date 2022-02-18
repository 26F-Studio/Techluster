//
// Created by particleg on 2021/9/27.
//

#include <filters/HeartBeatReport.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <magic_enum.hpp>
#include <structures/Exceptions.h>
#include <utils/http.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::filters;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

HeartBeatReport::HeartBeatReport() : I18nHelper(CMAKE_PROJECT_NAME) {}

void HeartBeatReport::doFilter(
        const HttpRequestPtr &req,
        FilterCallback &&failedCb,
        FilterChainCallback &&nextCb
) {
    try {
        auto request = RequestJson(req);
        request.require("ip", JsonValue::String);
        request.require("port", JsonValue::Uint);
        request.require("taskInterval", JsonValue::Double);
        request.require("description", JsonValue::String);
        request.require("info", JsonValue::Object);
        req->attributes()->insert(
                "requestJson",
                request
        );
        nextCb();
    } catch (const json_exception::InvalidFormat &e) {
        ResponseJson response;
        response.setResult(ResultCode::invalidFormat);
        response.setMessage(i18n("invalidFormat"));
        response.setReason(e);
        http::fromJson(
                k400BadRequest,
                response.ref(),
                failedCb
        );
    } catch (const json_exception::WrongType &e) {
        ResponseJson response;
        response.setResult(ResultCode::invalidArguments);
        response.setMessage(i18n("invalidArguments"));
        http::fromJson(
                k400BadRequest,
                response.ref(),
                failedCb
        );
    } catch (const exception &e) {
        ResponseJson response;
        LOG_ERROR << e.what();
        response.setResult(ResultCode::internalError);
        response.setMessage(i18n("internalError"));
        response.setReason(e);
        http::fromJson(
                k500InternalServerError,
                response.ref(),
                failedCb
        );
    }
}
