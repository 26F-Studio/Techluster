//
// Created by ParticleG on 2022/2/8.
//

#include <structures/ExceptionHandlers.h>

using namespace drogon;
using namespace std;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;

ResponseJsonHandler::ResponseJsonHandler(
        ResponseJsonHandler::ResponseExceptionHandler responseExceptionHandler,
        ResponseJsonHandler::DbExceptionHandler dbExceptionHandler,
        ResponseJsonHandler::GenericExceptionHandler genericExceptionHandler
) : _responseExceptionHandler(move(responseExceptionHandler)),
    _dbExceptionHandler(move(dbExceptionHandler)),
    _genericExceptionHandler(move(genericExceptionHandler)) {}

void ResponseJsonHandler::handleExceptions(
        const function<void()> &mainFunction,
        ResponseJson &response
) {
    try {
        mainFunction();
    } catch (const ResponseException &e) {
        _responseExceptionHandler(e, response);
    } catch (const orm::DrogonDbException &e) {
        _dbExceptionHandler(e, response);
    } catch (const exception &e) {
        _genericExceptionHandler(e, response);
    }
}