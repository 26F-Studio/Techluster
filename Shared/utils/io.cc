//
// Created by particleg on 2021/9/27.
//

#include <fstream>
#include <utils/io.h>

using namespace std;
using namespace tech::utils;

string io::getFileContent(string_view path) {
    constexpr auto readSize = size_t{4096};
    auto stream = ifstream{path.data()};
    stream.exceptions(ios_base::badbit);

    auto out = string{};
    auto buf = string(readSize, '\0');
    while (stream.read(&buf[0], readSize)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}
