//
// Created by ParticleG on 2022/3/23.
//

#include <types/NetEndian.h>

using namespace drogon;
using namespace std;
using namespace tech::types;
using namespace trantor;

NetEndian::NetEndian(InetAddress address) : address(address) {
    netEndian = (static_cast<uint64_t>(address.ipNetEndian()) << 32) + address.portNetEndian();
}
