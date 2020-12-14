//
// Created by liao xiangsen on 12/14/20.
//

#include <iostream>
#include <fstream>

#include "base/encoder.h"

TRACKER_USING

std::string JSONBase::Serialize() const {
    rapidjson::StringBuffer ss;
    rapidjson::Writer<rapidjson::StringBuffer> writer(ss);
    if (Serialize(&writer))
        return ss.GetString();
    return "";
}

bool JSONBase::Deserialize(const std::string& s) {
    rapidjson::Document doc;
    if (!InitDocument(s, doc))
        return false;
    Deserialize(doc);
    return true;
}

bool JSONBase::DeserializeFromFile(const std::string& filePath)
{
    std::ifstream f(filePath);
    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    return Deserialize(buffer.str());
}

bool JSONBase::SerializeToFile(const std::string& filePath) {
    std::ofstream f(filePath);
    std::string s = Serialize();
    f << s;
    f.flush();
    f.close();

    return true;
}

bool JSONBase::InitDocument(const std::string& s, rapidjson::Document& doc) {
    if (s.empty())
        return false;
    return !doc.Parse(s.c_str()).HasParseError();
}


