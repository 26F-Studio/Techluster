//
// Created by ParticleG on 2022/2/8.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/I18nHelper.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <structures/Exceptions.h>

namespace tech::structures {
    class RequestJsonHandler : public helpers::I18nHelper<RequestJsonHandler> {
    public:
        RequestJsonHandler();

        void handleExceptions(
                const std::function<void()> &mainFunction,
                drogon::FilterCallback &failedCb
        );

        virtual ~RequestJsonHandler() = default;
    };

    class ResponseJsonHandler {
    private:
        using ResponseExceptionHandler = std::function<void(const ResponseException &, helpers::ResponseJson &, drogon::HttpStatusCode &)>;
        using DbExceptionHandler = std::function<void(const drogon::orm::DrogonDbException &, helpers::ResponseJson &, drogon::HttpStatusCode &)>;
        using GenericExceptionHandler = std::function<void(const std::exception &, helpers::ResponseJson &, drogon::HttpStatusCode &)>;

    public:
        ResponseJsonHandler() = delete;

        ResponseJsonHandler(const ResponseJsonHandler &) = delete;

        ResponseJsonHandler(ResponseJsonHandler &&) = delete;

        ResponseJsonHandler(
                ResponseExceptionHandler responseExceptionHandler,
                DbExceptionHandler dbExceptionHandler,
                GenericExceptionHandler genericExceptionHandler
        );

        void handleExceptions(
                const std::function<void()> &mainFunction,
                helpers::ResponseJson &response,
                drogon::HttpStatusCode &code
        );

        virtual ~ResponseJsonHandler() = default;

    private:
        const ResponseExceptionHandler _responseExceptionHandler;
        const DbExceptionHandler _dbExceptionHandler;
        const GenericExceptionHandler _genericExceptionHandler;
    };
}
