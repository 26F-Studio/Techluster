//
// Created by ParticleG on 2022/2/9.
//

#include <helpers/BasicJson.h>

using namespace std;
using namespace tech::helpers;

BasicJson::BasicJson() : _value(Json::objectValue) {}

BasicJson::BasicJson(Json::Value json) : _value(std::move(json)) {}

BasicJson::BasicJson(const string &raw) { stringstream(raw) >> _value; }

string BasicJson::stringify(const string &indentation) const {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = indentation;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    std::ostringstream oss;
    jsonWriter->write(_value, &oss);
    return oss.str();
}

Json::Value &BasicJson::ref() { return _value; }

Json::Value BasicJson::copy() const { return _value; }

void BasicJson::operator()(Json::Value &&another) { _value = move(another); }

const Json::Value &BasicJson::operator[](Json::ArrayIndex index) const { return _value[index]; }

const Json::Value &BasicJson::operator[](int index) const { return _value[index]; }

const Json::Value &BasicJson::operator[](const char *key) const { return _value[key]; }

const Json::Value &BasicJson::operator[](const Json::String &key) const { return _value[key]; }