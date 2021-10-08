//
// Created by particleg on 2021/10/8.
//

#include <structures/Player.h>
#include <utils/crypto.h>

using namespace drogon;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

bool Player::isInRoom() const { return _inRoom; }

void Player::setInRoom(const bool &inRoom) { _inRoom = inRoom; }
