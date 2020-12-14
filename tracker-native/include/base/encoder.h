//
// Created by liao xiangsen on 11/27/20.
//

#ifndef ENCODER_H
#define ENCODER_H

#include <vector>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "base/utils.h"

TRACKER_BEGIN

class JSONBase {
public:
  bool DeserializeFromFile(const std::string& filePath);
  bool SerializeToFile(const std::string& filePath);

  virtual std::string Serialize() const;
  virtual bool Deserialize(const std::string& s);
  virtual bool Deserialize(const rapidjson::Value& obj) = 0;
  virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const = 0;
protected:
  bool InitDocument(const std::string &s, rapidjson::Document &doc);
};

inline const char* basicReply(bool result) {
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartObject();
    writer.Key("result");
    writer.Bool(result);
    writer.EndObject();

    return sb.GetString();
}

// SFINAE test
template <typename T>
class CanSerialize {
  private:
    typedef char YesType[1];
    typedef char NoType[2];

    template <typename C> static YesType& test(decltype(&C::Serialize)) ;
    template <typename C> static NoType& test(...);

  public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template <typename T>
typename std::enable_if<CanSerialize<T>::value, void>::type
arrayReply(const std::vector<T> &vals, rapidjson::Writer<rapidjson::StringBuffer> *writer) {
    writer->StartArray();
    for (const auto &val : vals) {
        val.Serialize(&writer);
    }
    writer->EndArray();
}

TRACKER_END

#endif //ENCODER_H
