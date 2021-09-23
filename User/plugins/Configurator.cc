//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/Configurator.h>

using namespace drogon;
using namespace std;
using namespace tech::plugins;

void Configurator::initAndStart(const Json::Value &config) {
    LOG_INFO << "Configurator loaded.";
}

void Configurator::shutdown() { LOG_INFO << "Configurator shutdown."; }