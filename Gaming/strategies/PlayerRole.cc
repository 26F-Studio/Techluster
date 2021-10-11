//
// Created by Particle_G on 2021/3/04.
//

#include <strategies/PlayerRole.h>

using namespace drogon;
using namespace std;
using namespace tech::strategies;
using namespace tech::structures;

PlayerRole::PlayerRole() {

}

Result PlayerRole::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response, CloseCode &code
) {
    return Result::error;
}
