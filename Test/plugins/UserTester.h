//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/plugins/Plugin.h>
#include <helpers/EmailHelper.h>
#include <plugins/BaseTester.h>

// TODO: Add failure tests
namespace tech::plugins {
    class UserTester : public BaseTester<UserTester> {
    public:
        UserTester();

        void processConfig(const Json::Value &config) override;

        bool verifyCode();

        bool loginCode();

        bool resetPassword();

        bool loginPassword();

        bool checkToken();

        bool refreshToken();

        bool migrateEmail();


        bool updateInfo();

        bool resetInfo();

        bool getAvatar();

        bool updateFullData();

        bool updatePartialData();

        bool updateSkipData();

        bool updateOverwriteData();

    private:
        std::unique_ptr<helpers::EmailHelper> _emailHelper;
        std::regex _codeRegex = std::regex(R"(>([0-9A-Z]{8})<)");
        std::string _verifyCode;

        int64_t _userId{};
        std::string _email;
        std::string _password;
        std::string _accessToken;
        std::string _refreshToken;

        Json::Value _emptyInfo, _info;
        Json::Value _fullData, _partialData, _skipData, _overwriteData;

        Json::Value _getInfo();

        Json::Value _getData(const std::string &path = "");

        static bool _compareJson(const Json::Value &a, const Json::Value &b);
    };
}

