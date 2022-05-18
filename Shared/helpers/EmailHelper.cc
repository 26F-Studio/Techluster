//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <mailio/pop3.hpp>
#include <mailio/smtp.hpp>
#include <helpers/EmailHelper.h>
#include <structures/Exceptions.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace mailio;
using namespace std;
using namespace tech::helpers;
using namespace tech::structures;
using namespace tech::types;
using namespace tech::utils;

EmailHelper::EmailHelper(
        string server,
        string account,
        string password,
        const string &senderEmail,
        const string &senderName
) : _server(move(server)),
    _account(move(account)),
    _password(move(password)),
    _sender(senderName, senderEmail) {}

message EmailHelper::pop3(
        uint32_t messageNumber,
        bool remove
) {
    message msg;
    msg.line_policy(
            codec::line_len_policy_t::RECOMMENDED,
            codec::line_len_policy_t::MANDATORY
    );
    pop3s conn(_server, 995);
    conn.authenticate(_account, _password, pop3s::auth_method_t::LOGIN);
    const auto &messageCount = conn.statistics().messages_no;
    const auto &number = (messageNumber == 0 || messageNumber > messageCount) ?
                         messageCount : messageNumber;
    conn.fetch(number, msg);
    if (remove) {
        conn.remove(number);
    }
    return msg;
}

void EmailHelper::smtp(
        const string &receiver,
        const string &subject,
        const string &content
) {
    try {
        message msg;
        msg.header_codec(message::header_codec_t::QUOTED_PRINTABLE);
        msg.from(_sender);
        msg.add_recipient({{}, receiver});
        msg.subject(subject);
        msg.content_transfer_encoding(mime::content_transfer_encoding_t::QUOTED_PRINTABLE);
        msg.content_type(
                message::media_type_t::TEXT,
                "html",
                "utf-8"
        );
        msg.content(content);
        smtps conn(_server, 587);
        conn.authenticate(
                _account,
                _password,
                smtps::auth_method_t::START_TLS
        );
        conn.submit(msg);
    } catch (smtp_error &e) {
        LOG_WARN << "SMTP Error: " << e.what();
        throw ResponseException(
                i18n("emailError"),
                e,
                ResultCode::EmailError,
                k503ServiceUnavailable
        );
    } catch (dialog_error &e) {
        LOG_WARN << "Dialog Error: " << e.what();
        throw ResponseException(
                i18n("emailError"),
                e,
                ResultCode::EmailError,
                k503ServiceUnavailable
        );
    } catch (message_error &e) {
        LOG_WARN << "Message Error: " << e.what();
        throw ResponseException(
                i18n("emailError"),
                e,
                ResultCode::EmailError,
                k503ServiceUnavailable
        );
    }
}
