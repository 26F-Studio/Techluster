//
// Created by Parti on 2021/2/4.
//

#include <plugins/Configurator.h>

using namespace std;
using namespace tech::plugins;

void Configurator::initAndStart(const Json::Value &config) {
    if (config.isMember("superusers") && config["superusers"].isArray()) {
        for (const auto &superuser : config["superusers"]) {
            _superusers.insert(superuser.asInt64());
        }
    }
    if (config.isMember("credential") && config["credential"].isString()) {
        _credential = config["credential"].asString();
    } else {
        LOG_ERROR << R"("Invalid credential config")";
        abort();
    }
    LOG_INFO << "Configurator loaded.";
}

void Configurator::shutdown() { LOG_INFO << "Configurator shutdown."; }

bool Configurator::isSuperusers(const int64_t &uid) const { return _superusers.contains(uid); }

bool Configurator::checkCredential(const string &credential) const { return credential == _credential; }