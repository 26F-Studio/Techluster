//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>

namespace tech::plugins {
    class EmailHelper : public drogon::Plugin<EmailHelper> {
    public:
        EmailHelper() = default;

        void initAndStart(const Json::Value &config) override;

        void shutdown() override;

        void smtp(
                const std::string &receiver,
                const std::string &subject,
                const std::string &content
        );

    private:
        uint32_t _emailPort{};
        std::string _emailAddress{},
                _emailHost{},
                _emailUsername{},
                _emailPassword{},
                _emailName{};
    };
}

