//
// Created by Particle_G on 2021/9/4.
//

#include <utils/serializer.h>

using namespace tech::utils;
using namespace std;

string serializer::json::stringify(
        const Json::Value &value,
        const string &indentation
) {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = indentation;
    unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    ostringstream oss;
    jsonWriter->write(value, &oss);
    return oss.str();
}

Json::Value serializer::json::parse(const string &raw) {
    Json::Value result;
    stringstream(raw) >> result;
    return result;
}
