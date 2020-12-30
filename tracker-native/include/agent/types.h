//
// Created by liao xiangsen on 12/10/20.
//

#ifndef AGENT_TYPES_H
#define AGENT_TYPES_H

#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <frida-gum.h>
#include <gumpp.hpp>

#include "base/utils.h"
#include "base/encoder.h"

TRACKER_AGENT_BEGIN

struct FridaContext {
  GumStalker *stalker;
  Gum::Interceptor *interceptor;
  // list of modules in the target process
  GumModuleMap *moduleMap;
  pthread_key_t keyToCoverageInfo;
};

class ThreadInfo : public tracker::JSONBase {
public:
  int tid;
  std::string name;
  // pair<module, symbol>
  std::vector<std::pair<std::string, std::string>> backtrace;

  ThreadInfo();
  virtual ~ThreadInfo();
  ThreadInfo(const ThreadInfo &other);
  bool Deserialize(const rapidjson::Value &obj) override;
  bool Serialize(rapidjson::Writer<rapidjson::StringBuffer> *writer) const override;
};

typedef uintptr_t BlockID;
typedef uintptr_t BranchID;
typedef uint32_t Count;

#if defined(__LP64__)
#define GetBranchID(x,y) ((x << 32 | x >> 32) ^ y)
#else
#define GetBranchID(x,y) ((x << 16 | x >> 16) ^ y)
#endif

const int PRINT_MAX_LEN = 20;

//#define COLLECT_BRANCH_COVERAGE 1

struct ThreadCoverageInfo {
public:
  int tid;
  std::map<BlockID, Count> blockCounter;
#ifdef COLLECT_BRANCH_COVERAGE
  std::map<BranchID, Count> branchCounter;
  BlockID prevBlockID = 0;
#endif

  ThreadCoverageInfo(int _tid): tid(_tid) {}
  void incCounter(BlockID curBlockID);
  bool Serialize(rapidjson::Writer<rapidjson::StringBuffer> *writer) const;
  std::string print() const;
};

class Interceptor : public tracker::JSONBase {
public:
  std::string moduleName;
  std::string funcSymbolName;
  uintptr_t funcAddress; // optional

  Interceptor();
  virtual ~Interceptor();
  Interceptor(const Interceptor &other);
  bool Deserialize(const rapidjson::Value &obj) override;
  bool Serialize(rapidjson::Writer<rapidjson::StringBuffer> *writer) const override;
};

TRACKER_AGENT_END

#endif //AGENT_TYPES_H
