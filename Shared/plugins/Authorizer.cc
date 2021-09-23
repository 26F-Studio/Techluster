//
// Created by Particle_G on 2021/9/9.
//

#include <drogon/drogon.h>
#include <plugins/Authorizer.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;

void Authorizer::initAndStart(const Json::Value &config) {
    if (config.isMember("superusers") && config["superusers"].isArray()) {
        for (const auto &superuser: config["superusers"]) {
            _superusers.insert(superuser.asInt64());
        }
    }

    if (config.isMember("credential") && config["credential"].isString()) {
        _credential = config["credential"].asString();
    } else {
        LOG_ERROR << R"("Invalid credential config")";
        abort();
    }

    LOG_INFO << "Authorizer loaded.";
}

void Authorizer::shutdown() { LOG_INFO << "Authorizer shutdown."; }

bool Authorizer::isSuperusers(const int64_t &uid) const { return _superusers.contains(uid); }

string Authorizer::getCredential() const { return _credential; }

bool Authorizer::checkCredential(const string &credential) const { return credential == _credential; }
