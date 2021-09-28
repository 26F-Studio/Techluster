//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/Configurator.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;

void Configurator::initAndStart(const Json::Value &config) {
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

    LOG_INFO << "Configurator loaded.";
}

void Configurator::shutdown() { LOG_INFO << "Configurator shutdown."; }

string Configurator::getEmailUsername() const { return _emailUsername; }

string Configurator::getEmailPassword() const { return _emailPassword; }

string Configurator::getEmailAddress() const { return _emailAddress; }

string Configurator::getEmailName() const { return _emailName; }

uint32_t Configurator::getEmailPort() const { return _emailPort; }

string Configurator::getEmailHost() const { return _emailHost; }
