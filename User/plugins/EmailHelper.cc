//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <mailio/message.hpp>
#include <mailio/smtp.hpp>
#include <plugins/EmailHelper.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace mailio;
using namespace std;
using namespace tech::plugins;
using namespace tech::utils;

void EmailHelper::initAndStart(const Json::Value &config) {
    if (!(
            config.isMember("smtp") && config["smtp"].isObject() &&
            config["smtp"].isMember("username") && config["smtp"]["username"].isString() &&
            config["smtp"].isMember("password") && config["smtp"]["password"].isString() &&
            config["smtp"].isMember("address") && config["smtp"]["address"].isString() &&
            config["smtp"].isMember("name") && config["smtp"]["name"].isString() &&
            config["smtp"].isMember("port") && config["smtp"]["port"].isUInt() &&
            config["smtp"].isMember("host") && config["smtp"]["host"].isString()
    )) {
        LOG_ERROR << R"(Invalid smtp config)";
        abort();
    } else {
        _emailUsername = config["smtp"]["username"].asString();
        _emailPassword = config["smtp"]["password"].asString();
        _emailAddress = config["smtp"]["address"].asString();
        _emailName = config["smtp"]["name"].asString();
        _emailPort = config["smtp"]["port"].asUInt();
        _emailHost = config["smtp"]["host"].asString();
    }

    LOG_INFO << "EmailHelper loaded.";
}

void EmailHelper::shutdown() { LOG_INFO << "EmailHelper shutdown."; }

void EmailHelper::smtp(
        const string &receiver,
        const string &subject,
        const string &content
) {
    message msg;
    msg.header_codec(message::header_codec_t::QUOTED_PRINTABLE);
    msg.from(mail_address(
            _emailName,
            _emailAddress
    ));
    msg.add_recipient(
            {
                    "email_" + crypto::blake2B(receiver, 4),
                    receiver
            }
    );
    msg.subject(subject);
    msg.content_transfer_encoding(mime::content_transfer_encoding_t::QUOTED_PRINTABLE);
    msg.content_type(
            message::media_type_t::TEXT,
            "html",
            "utf-8"
    );
    msg.content(content);
    smtps conn(_emailHost, _emailPort);
    conn.authenticate(
            _emailUsername,
            _emailPassword,
            smtps::auth_method_t::START_TLS
    );
    conn.submit(msg);
}
