//
// Created by ParticleG on 2022/2/9.
//

#include <helpers/ResponseJson.h>
#include <utils/data.h>

using namespace std;
using namespace tech::helpers;
using namespace tech::types;
using namespace tech::utils::data;

void ResponseJson::setResult(const ResultCode &code) { setResult(+code); }

void ResponseJson::setResult(const uint32_t &code) { _value["code"] = code; }

void ResponseJson::setMessage(const string &message) { _value["message"] = message; }

void ResponseJson::setData(Json::Value data) { _value["data"] = move(data); }

void ResponseJson::setReason(const exception &e) { setReason(e.what()); }

void ResponseJson::setReason(const drogon::orm::DrogonDbException &e) { setReason(e.base().what()); }

void ResponseJson::setReason(const string &reason) { _value["reason"] = reason; }

