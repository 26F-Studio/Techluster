//
// Created by Particle_G on 2021/3/04.
//

#include <strategies/PlayerType.h>

using namespace drogon;
using namespace std;
using namespace tech::strategies;
using namespace tech::structures;

PlayerType::PlayerType() {

}

Result PlayerType::fromJson(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &request,
        Json::Value &response, CloseCode &code
) {
    return Result::error;
}
