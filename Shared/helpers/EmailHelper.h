//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/drogon.h>
#include <helpers/I18nHelper.h>
#include <mailio/mailboxes.hpp>
#include <mailio/message.hpp>

namespace tech::helpers {
    class EmailHelper : public I18nHelper<EmailHelper> {
    public:
        static constexpr char projectName[] = CMAKE_PROJECT_NAME;

    public:
        EmailHelper(
                std::string server,
                std::string account,
                std::string password,
                const std::string &senderEmail = "",
                const std::string &senderName = ""
        );

        mailio::message pop3(
                uint32_t messageNumber = 0,
                bool remove = true
        );

        void smtp(
                const std::string &receiver,
                const std::string &subject,
                const std::string &content
        );

    private:
        std::string _server;
        mailio::mail_address _sender;
        std::string _account, _password;
    };
}

