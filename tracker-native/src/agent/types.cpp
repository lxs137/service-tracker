//
// Created by liao xiangsen on 12/14/20.
//

#include "agent/types.h"

TRACKER_AGENT_USING

ThreadInfo::ThreadInfo(): tid(0), name("unknown") {}

ThreadInfo::~ThreadInfo() {}

ThreadInfo::ThreadInfo(const ThreadInfo &other) {
    tid = other.tid;
    name = std::string(other.name);

    for (const auto &it : other.backtrace) {
        backtrace.emplace_back(it.first, it.second);
    }
}

bool ThreadInfo::Deserialize(const rapidjson::Value &obj) {
    this->tid = obj["tid"].GetInt();
    this->name = std::string(obj["name"].GetString());
    return true;
}

bool ThreadInfo::Serialize(rapidjson::Writer<rapidjson::StringBuffer> *writer) const {
    writer->StartObject();

    writer->String("tid");
    writer->Int(this->tid);

    writer->String("name");
    writer->String(this->name.c_str());

    writer->String("backtrace");
    writer->StartArray();
    for (const auto &it : this->backtrace) {
        writer->StartObject();
        writer->String("module");
        writer->String(it.first.c_str(), it.first.size());
        writer->String("symbol");
        writer->String(it.second.c_str(), it.second.size());
        writer->EndObject();
    }
    writer->EndArray();

    writer->EndObject();
    return true;
}