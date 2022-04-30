//
// Created by ParticleG on 2022/2/8.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/I18nHelper.h>
#include <helpers/MessageJson.h>
#include <helpers/RequestJson.h>
#include <helpers/ResponseJson.h>
#include <structures/Exceptions.h>

namespace tech::structures {
    template<class T>
    class RequestJsonHandler : public helpers::I18nHelper<T> {
    public:
        void handleExceptions(
                const std::function<void()> &mainFunction,
                drogon::FilterCallback &failedCb
        ) {
            using namespace drogon;
            using namespace std;
            using namespace tech::helpers;
            using namespace tech::structures;
            using namespace tech::types;

            try {
                mainFunction();
            } catch (const json_exception::InvalidFormat &e) {
                ResponseJson response;
                response.setStatusCode(k400BadRequest);
                response.setResultCode(ResultCode::invalidFormat);
                // message.setMessage(this->i18n(e.what()));
                response.setMessage(I18nHelper<T>::i18n("invalidFormat"));
                response.setReason(e);
                response.httpCallback(failedCb);
            } catch (const json_exception::WrongType &e) {
                ResponseJson response;
                response.setStatusCode(k400BadRequest);
                response.setResultCode(ResultCode::invalidArguments);
                response.setMessage(I18nHelper<T>::i18n("invalidArguments"));
                response.httpCallback(failedCb);
            } catch (const exception &e) {
                LOG_ERROR << e.what();
                ResponseJson response;
                response.setStatusCode(k500InternalServerError);
                response.setResultCode(ResultCode::internalError);
                response.setMessage(I18nHelper<T>::i18n("internalError"));
                response.setReason(e);
                response.httpCallback(failedCb);
            }
        }

        ~RequestJsonHandler() override = default;
    };

    class ResponseJsonHandler {
    private:
        using ResponseExceptionHandler = std::function<void(const ResponseException &, helpers::ResponseJson &)>;
        using DbExceptionHandler = std::function<void(const drogon::orm::DrogonDbException &, helpers::ResponseJson &)>;
        using GenericExceptionHandler = std::function<void(const std::exception &, helpers::ResponseJson &)>;

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
                helpers::ResponseJson &response
        );

        virtual ~ResponseJsonHandler() = default;

    private:
        const ResponseExceptionHandler _responseExceptionHandler;
        const DbExceptionHandler _dbExceptionHandler;
        const GenericExceptionHandler _genericExceptionHandler;
    };

    template<class T>
    class MessageJsonHandler : public helpers::I18nHelper<T> {
    public:
        void handleExceptions(
                const std::function<void()> &mainFunction,
                int action,
                const drogon::WebSocketConnectionPtr &wsConnPtr
        ) {
            using namespace drogon;
            using namespace std;
            using namespace tech::helpers;
            using namespace tech::structures;
            using namespace tech::types;

            try {
                mainFunction();
            } catch (const MessageException &e) {
                MessageJson message(action);
                message.setReason(I18nHelper<T>::i18n(e.what()));
                if (e.error) {
                    message.setMessageType(MessageType::error);
                    wsConnPtr->shutdown(CloseCode::kViolation, message.stringify());
                } else {
                    message.setMessageType(MessageType::failed);
                    wsConnPtr->send(message.stringify());
                }
            } catch (const orm::DrogonDbException &e) {
                LOG_ERROR << e.base().what();
                MessageJson message(action);
                message.setReason(I18nHelper<T>::i18n("databaseError"));
                message.setMessageType(MessageType::error);
                wsConnPtr->shutdown(CloseCode::kViolation, message.stringify());
            } catch (const exception &e) {
                LOG_ERROR << e.what();
                MessageJson message(action);
                message.setReason(I18nHelper<T>::i18n("internalError"));
                message.setMessageType(MessageType::error);
                wsConnPtr->shutdown(CloseCode::kViolation, message.stringify());
            }
        }

        virtual ~MessageJsonHandler() = default;
    };
}