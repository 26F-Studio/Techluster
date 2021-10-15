//
// Created by Particle_G on 2021/8/19.
//

#include <mailio/message.hpp>
#include <mailio/smtp.hpp>
#include <services/Auth.h>
#include <structures/Exceptions.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace mailio;
using namespace std;
using namespace tech::plugins;
using namespace tech::services;
using namespace tech::structures;
using namespace tech::utils;

Auth::Auth() : _configurator(app().getPlugin<Configurator>()),
               _dataManager(app().getPlugin<DataManager>()) {}

Json::Value Auth::check(HttpStatusCode &code, const string &accessToken) {
    Json::Value response;
    try {
        response["type"] = "Success";
        response["data"] = _dataManager->getUserId(accessToken);
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const exception &e) {
        LOG_ERROR << "error:" << e.what();
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value Auth::refresh(HttpStatusCode &code, const string &refreshToken) {
    Json::Value response;
    try {
        auto tokens = _dataManager->refresh(refreshToken);
        response["type"] = "Success";
        response["data"]["refreshToken"] = tokens.refresh();
        response["data"]["accessToken"] = tokens.access();
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const exception &e) {
        LOG_ERROR << "error:" << e.what();
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value Auth::verifyEmail(HttpStatusCode &code, const Json::Value &data) {
    Json::Value response;
    auto email = data["email"].asString();
    auto verifyCode = _dataManager->verifyEmail(email);
    auto mailContent = _getFileContent("./verifyEmail.html");
    drogon::utils::replaceAll(
            mailContent,
            "{{VERIFY_CODE}}",
            verifyCode
    );
    message msg;
    try {
        msg.header_codec(message::header_codec_t::BASE64);
        msg.from(mail_address(
                _configurator->getEmailName(),
                _configurator->getEmailAddress()
        ));
        msg.add_recipient(mail_address(
                "email_" + crypto::blake2B(email, 4),
                email
        ));
        msg.subject("[Techmino] Verify Code");
        msg.content_transfer_encoding(mime::content_transfer_encoding_t::QUOTED_PRINTABLE);
        msg.content_type(message::media_type_t::TEXT, "html", "utf-8");
        msg.content(mailContent);
        smtps conn(_configurator->getEmailHost(), _configurator->getEmailPort());
        conn.authenticate(_configurator->getEmailUsername(), _configurator->getEmailPassword(), smtps::auth_method_t::START_TLS);
        conn.submit(msg);
        response["type"] = "Success";
    } catch (smtp_error &e) {
        code = k503ServiceUnavailable;
        LOG_ERROR << e.what();
        response["type"] = "Error";
        response["reason"] = e.what();
    } catch (dialog_error &e) {
        code = k503ServiceUnavailable;
        LOG_ERROR << e.what();
        response["type"] = "Error";
        response["reason"] = e.what();
    } catch (message_error &e) {
        code = k503ServiceUnavailable;
        LOG_ERROR << e.what();
        response["type"] = "Error";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value Auth::loginMail(HttpStatusCode &code, const Json::Value &data) {
    Json::Value response;
    auto email = data["email"].asString();
    auto validCode = data["code"].asString();
    auto password = data["password"].asString();
    try {
        if (!validCode.empty()) {
            auto tokens = _dataManager->loginEmailCode(email, validCode);
            response["type"] = "Success";
            response["data"]["refreshToken"] = tokens.refresh();
            response["data"]["accessToken"] = tokens.access();
        } else if (!password.empty()) {
            auto tokens = _dataManager->loginEmailPassword(email, password);
            response["type"] = "Success";
            response["data"]["refreshToken"] = tokens.refresh();
            response["data"]["accessToken"] = tokens.access();
        } else {
            code = k400BadRequest;
            response["type"] = "Error";
            response["reason"] = "Invalid parameters";
        }
    } catch (const redis_exception::KeyNotFound &e) {
        code = k403Forbidden;
        response["type"] = "Failed";
        response["reason"] = "Invalid verify code";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        code = drogon::k503ServiceUnavailable;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value Auth::resetEmail(HttpStatusCode &code, const Json::Value &data) {
    auto email = data["email"].asString();
    auto validCode = data["code"].asString();
    auto newPassword = data["newPassword"].asString();
    Json::Value response;
    try {
        _dataManager->resetEmail(email, validCode, newPassword);
        response["type"] = "Success";
    } catch (const redis_exception::KeyNotFound &e) {
        code = k403Forbidden;
        response["type"] = "Failed";
        response["reason"] = "Invalid verify code";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "ORM error";
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        code = drogon::k503ServiceUnavailable;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

Json::Value Auth::migrateEmail(HttpStatusCode &code, const Json::Value &data) {
    auto accessToken = data["accessToken"].asString();
    auto newEmail = data["newEmail"].asString();
    auto validCode = data["code"].asString();
    Json::Value response;
    try {
        _dataManager->migrateEmail(
                accessToken,
                newEmail,
                validCode
        );
        response["type"] = "Success";
    } catch (const redis_exception::KeyNotFound &e) {
        code = k401Unauthorized;
        response["type"] = "Failed";
        response["reason"] = "Invalid access token";
    } catch (const redis_exception::NotEqual &e) {
        code = k403Forbidden;
        response["type"] = "Failed";
        response["reason"] = "Invalid verify code";
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        code = k500InternalServerError;
        response["type"] = "Error";
        response["reason"] = "Database error";
    } catch (const exception &e) {
        LOG_ERROR << e.what();
        code = k503ServiceUnavailable;
        response["type"] = "Failed";
        response["reason"] = e.what();
    }
    return response;
}

string Auth::_getFileContent(string_view path) {
    constexpr auto readSize = size_t{4096};
    auto stream = ifstream{path.data()};
    stream.exceptions(ios_base::badbit);

    auto out = string{};
    auto buf = string(readSize, '\0');
    while (stream.read(&buf[0], readSize)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}
