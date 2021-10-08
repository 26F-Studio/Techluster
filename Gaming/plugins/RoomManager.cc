//
// Created by Parti on 2021/2/4.
//

#include <drogon/drogon.h>
#include <plugins/RoomManager.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace std;
using namespace sw::redis;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;

void RoomManager::initAndStart(const Json::Value &config) {
    LOG_INFO << "RoomManager loaded.";
}

void RoomManager::shutdown() { LOG_INFO << "RoomManager shutdown."; }

string RoomManager::createRoom(
        const uint32_t &capacity,
        const string &password,
        const Json::Value &info,
        const Json::Value &config
) {
    return _redisHelper->createRoom(
            capacity,
            crypto::blake2b(password),
            info,
            config
    );
}

void RoomManager::joinRoom(const string &roomId) {

}

