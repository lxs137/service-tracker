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

#include "frida-gum.h"

#include "base/utils.h"
#include "base/encoder.h"

TRACKER_AGENT_BEGIN

struct FridaContext {
  GumStalker *stalker;
  // list of modules in the target process
  GumModuleMap *moduleMap;
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

struct ThreadCoverageInfo {
public:
  int tid;
  // stats
  std::map<BlockID, Count> blockCounter;
  std::map<BranchID, Count> branchCounter;
  // temp
  BlockID prevBlockID;

  ThreadCoverageInfo(int _tid): tid(_tid), prevBlockID(0) {}
  void incCounter(BlockID curBlockID) {
      auto itBlock = blockCounter.find(curBlockID);
      if (itBlock == blockCounter.end()) {
          blockCounter.emplace(curBlockID, 1);
      } else {
          blockCounter[curBlockID] = itBlock->second + 1;
      }

      BranchID curBranchID = GetBranchID(curBlockID, prevBlockID);
      auto itBranch = branchCounter.find(curBranchID);
      if (itBranch == branchCounter.end()) {
          branchCounter.emplace(curBranchID, 1);
      } else {
          branchCounter[curBranchID] = itBranch->second + 1;
      }

      prevBlockID = curBlockID;
  }
  std::string print() const {
      int counter;
      std::ostringstream oss;
      oss << "Coverage(tid-" << tid << ") : {" << std::endl;
      if (!blockCounter.empty()) {
          counter = 0;
          oss << "    block counter: [ ";
          for (auto it : blockCounter) {
              oss << "(" << it.first << ", " << it.second << ") ";
              if (++counter > PRINT_MAX_LEN) {
                  break;
              }
          }
          oss << " ]" << std::endl;
      }
      if (!branchCounter.empty()) {
          counter = 0;
          oss << "    branch counter: [ ";
          for (auto it : branchCounter) {
              oss << "(" << it.first << ", " << it.second << ") ";
              if (++counter > PRINT_MAX_LEN) {
                  break;
              }
          }
          oss << " ]" << std::endl;
      }
      oss << "}";
      return oss.str();
  }
};

struct TransformerContext {
  ThreadCoverageInfo* coverageInfoPtr;
};


TRACKER_AGENT_END

#endif //AGENT_TYPES_H
