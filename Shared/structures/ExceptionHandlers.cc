//
// Created by ParticleG on 2022/2/8.
//

#include <structures/ExceptionHandlers.h>
#include <utils/http.h>

using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

RequestJsonHandler::RequestJsonHandler() : I18nHelper(CMAKE_PROJECT_NAME) {}

void RequestJsonHandler::handleExceptions(
        const function<void()> &mainFunction,
        drogon::FilterCallback &failedCb
) {
    try {
        mainFunction();
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

ResponseJsonHandler::ResponseJsonHandler(
        ResponseJsonHandler::ResponseExceptionHandler responseExceptionHandler,
        ResponseJsonHandler::DbExceptionHandler dbExceptionHandler,
        ResponseJsonHandler::GenericExceptionHandler genericExceptionHandler
) : _responseExceptionHandler(move(responseExceptionHandler)),
    _dbExceptionHandler(move(dbExceptionHandler)),
    _genericExceptionHandler(move(genericExceptionHandler)) {}

void ResponseJsonHandler::handleExceptions(
        const function<void()> &mainFunction,
        ResponseJson &response,
        drogon::HttpStatusCode &code
) {
    try {
        mainFunction();
    } catch (const ResponseException &e) {
        _responseExceptionHandler(e, response, code);
    } catch (const orm::DrogonDbException &e) {
        _dbExceptionHandler(e, response, code);
    } catch (const exception &e) {
        _genericExceptionHandler(e, response, code);
    }
}
