//
// Created by ParticleG on 2022/2/6.
//

#include <utils/data.h>
#include <random>

using namespace std;
using namespace tech::utils;
using namespace trantor;

static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

string data::randomString(const uint64_t &length) {
    static mt19937 engine(random_device{}());
    static uniform_int_distribution<uint64_t> uniDist(0, sizeof(charset) - 2);
    string result;
    result.resize(length);
    for (auto &c: result) {
        c = charset[uniDist(engine)];
    }
    return result;
}
