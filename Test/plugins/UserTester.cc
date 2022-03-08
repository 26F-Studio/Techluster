//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <magic_enum.hpp>
#include <plugins/UserTester.h>
#include <types/ResultCode.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace magic_enum;
using namespace std;
using namespace tech::helpers;
using namespace tech::plugins;
using namespace tech::types;
using namespace tech::utils;

UserTester::UserTester() {
    using namespace Json;
    _emptyInfo["username"] = "Stacker";
    _emptyInfo["motto"] = "";
    _emptyInfo["region"] = 0;
    _emptyInfo["avatar"] = "";
    _emptyInfo["avatar_frame"] = 0;
    _emptyInfo["clan"] = "";

    _fullData[0]["path"] = "";
    _fullData[0]["value"]["unlocked"][0] = 0;
    _fullData[0]["value"]["unlocked"][1] = 1;
    _fullData[0]["value"]["highScores"][0]["level"] = 0;
    _fullData[0]["value"]["highScores"][0]["score"] = 0;
    _fullData[0]["value"]["highScores"][1]["level"] = 1000;
    _fullData[0]["value"]["highScores"][1]["score"] = 1000;

    _partialData[0]["path"] = "highScores.2";
    _partialData[0]["value"]["level"] = 2;
    _partialData[0]["value"]["score"] = 2000;

    _skipData[0]["path"] = "highScores.5";
    _skipData[0]["value"]["level"] = 5;
    _skipData[0]["value"]["score"] = 5000;

    _overwriteData[0]["path"] = "unlocked";
    _overwriteData[0]["value"]["levels"][0] = 0;
    _overwriteData[0]["value"]["levels"][1] = 1;
    _overwriteData[0]["value"]["levels"][2] = 2;
    _overwriteData[0]["value"]["levels"][3] = 3;
    _overwriteData[0]["value"]["levels"][4] = 4;
}

void UserTester::processConfig(const Json::Value &config) {
    _email = config["account"]["email"].asString();
    _password = config["account"]["password"].asString();
    _emailHelper = make_unique<EmailHelper>(
            config["smtp"]["server"].asString(),
            _email,
            config["smtp"]["password"].asString()
    );
    _info = config["info"];
}

bool UserTester::verifyCode() {
    WebHelper::colorOut("Sending verify email...", WebHelper::Color::cyan);
    Json::Value body;
    body["email"] = _email;
    auto response = _httpRequest(
            Post,
            "/auth/verify/email",
            {},
            {},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        WebHelper::colorOut("Wait 5 seconds for email delivery", WebHelper::Color::cyan);
        this_thread::sleep_for(std::chrono::seconds(5));
        WebHelper::colorOut("Receiving verify code...", WebHelper::Color::cyan);
        auto message = _emailHelper->pop3();
        auto content = message.content();
        smatch sm;
        regex_search(content, sm, _codeRegex);
        if (sm.size() == 2) {
            _verifyCode = sm[1];
            LOG_INFO << "Verify code: " << _verifyCode;
        } else {
            throw exception("Can not find verify code in email");
        }
        return true;
    }
    return false;
}

bool UserTester::loginCode() {
    WebHelper::colorOut("Login with email and verify code", WebHelper::Color::cyan);
    Json::Value body;
    body["email"] = _email;
    body["code"] = _verifyCode;
    auto response = _httpRequest(
            Post,
            "/auth/login/email",
            {},
            {},
            body
    );
    auto code = response["code"].asUInt();
    if (!(
            code == enum_integer(ResultCode::completed) ||
            code == enum_integer(ResultCode::continued)
    )) {
        return false;
    }
    _accessToken = response["data"]["accessToken"].asString();
    _refreshToken = response["data"]["refreshToken"].asString();
    return true;
}

bool UserTester::resetPassword() {
    WebHelper::colorOut("Login with email and verify code", WebHelper::Color::cyan);
    Json::Value body;
    body["email"] = _email;
    body["code"] = _verifyCode;
    body["newPassword"] = _password;
    auto response = _httpRequest(
            Put,
            "/auth/reset/email",
            {},
            {},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        _accessToken = response["data"]["accessToken"].asString();
        _refreshToken = response["data"]["refreshToken"].asString();
        return true;
    }
    return false;
}

bool UserTester::loginPassword() {
    WebHelper::colorOut("Login with email and password", WebHelper::Color::cyan);
    Json::Value body;
    body["email"] = _email;
    body["password"] = _password;
    auto response = _httpRequest(
            Post,
            "/auth/login/email",
            {},
            {},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        _accessToken = response["data"]["accessToken"].asString();
        _refreshToken = response["data"]["refreshToken"].asString();
        return true;
    }
    return false;
}

bool UserTester::checkToken() {
    WebHelper::colorOut("Check access token", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Get,
            "/auth/check",
            {},
            {make_pair("x-access-token", _accessToken)}
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        _userId = response["data"].asInt64();
        return true;
    }
    return false;
}

bool UserTester::refreshToken() {
    WebHelper::colorOut("Refresh access token", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Get,
            "/auth/refresh",
            {},
            {make_pair("x-refresh-token", _refreshToken)}
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        _accessToken = response["data"]["accessToken"].asString();
        _refreshToken = response["data"]["refreshToken"].asString();
        return true;
    }
    return false;
}

bool UserTester::migrateEmail() {
    WebHelper::colorOut("Migrate to new email", WebHelper::Color::cyan);
    Json::Value body;
    body["newEmail"] = _email;
    body["code"] = _verifyCode;
    auto response = _httpRequest(
            Put,
            "/auth/migrate/email",
            {},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    return response["code"].asUInt() == enum_integer(ResultCode::completed);
}

bool UserTester::updateInfo() {
    WebHelper::colorOut("Update player info", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Put,
            "/user/info",
            {},
            {make_pair("x-access-token", _accessToken)},
            _info
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _compareJson(_info, _getInfo());
    }
    return false;
}

bool UserTester::resetInfo() {
    WebHelper::colorOut("Reset player info", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Put,
            "/user/info",
            {},
            {make_pair("x-access-token", _accessToken)},
            _emptyInfo
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _compareJson(_emptyInfo, _getInfo());
    }
    return false;
}

bool UserTester::getAvatar() {
    WebHelper::colorOut("Get player avatar", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Get,
            "/user/avatar",
            {make_pair("userId", to_string(_userId))},
            {make_pair("x-access-token", _accessToken)}
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _info["avatar"].asString() == response["data"].asString();
    }
    return false;
}

bool UserTester::updateFullData() {
    WebHelper::colorOut("Update player full data", WebHelper::Color::cyan);
    Json::Value body;
    body["data"] = _fullData;
    auto response = _httpRequest(
            Put,
            "/user/data",
            {make_pair("dataField", "Public")},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _fullData[0]["value"] == _getData()[0];
    }
    return false;
}

bool UserTester::updatePartialData() {
    WebHelper::colorOut("Update player partial data", WebHelper::Color::cyan);
    Json::Value body;
    body["data"] = _partialData;
    auto response = _httpRequest(
            Put,
            "/user/data",
            {make_pair("dataField", "Public")},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _partialData[0]["value"] == _getData(_partialData[0]["path"].asString())[0];
    }
    return false;
}

bool UserTester::updateSkipData() {
    WebHelper::colorOut("Update player skip data", WebHelper::Color::cyan);
    Json::Value body;
    body["data"] = _skipData;
    body["options"]["skip"] = true;
    auto response = _httpRequest(
            Put,
            "/user/data",
            {make_pair("dataField", "Public")},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        auto highScores = _getData("highScores")[0];
        return _skipData[0]["value"] == highScores[5] &&
               highScores[3] == Json::nullValue &&
               highScores[4] == Json::nullValue;
    }
    return false;
}

bool UserTester::updateOverwriteData() {
    WebHelper::colorOut("Update player overwrite data", WebHelper::Color::cyan);
    Json::Value body;
    body["data"] = _overwriteData;
    body["options"]["overwrite"] = true;
    auto response = _httpRequest(
            Put,
            "/user/data",
            {make_pair("dataField", "Public")},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return _overwriteData[0]["value"] == _getData(_overwriteData[0]["path"].asString())[0];
    }
    return false;
}

Json::Value UserTester::_getInfo() {
    WebHelper::colorOut("Get player info", WebHelper::Color::cyan);
    auto response = _httpRequest(
            Get,
            "/user/info",
            {make_pair("userId", to_string(_userId))},
            {make_pair("x-access-token", _accessToken)}
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return response["data"];
    }
    return {};
}

Json::Value UserTester::_getData(const string &path) {
    WebHelper::colorOut("Get player data by path: " + path, WebHelper::Color::cyan);
    Json::Value body;
    body["paths"][0] = path;
    auto response = _httpRequest(
            Post,
            "/user/data",
            {make_pair("dataField", "Public")},
            {make_pair("x-access-token", _accessToken)},
            body
    );
    if (response["code"].asUInt() == enum_integer(ResultCode::completed)) {
        return response["data"];
    }
    return {};
}

bool UserTester::_compareJson(const Json::Value &a, const Json::Value &b) {
    return a["avatar_frame"].asInt64() == b["avatar_frame"].asInt64() &&
           crypto::blake2B(a["avatar"].asString()) == b["avatar_hash"].asString() &&
           a["clan"].asString() == b["clan"].asString() &&
           a["motto"].asString() == b["motto"].asString() &&
           a["region"].asInt64() == b["region"].asInt64() &&
           a["username"].asString() == b["username"].asString();
}
