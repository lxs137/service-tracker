//
// Created by liao xiangsen on 12/14/20.
//

#include "agent/types.h"

TRACKER_AGENT_USING

void ThreadCoverageInfo::incCounter(BlockID curBlockID) {
    auto itBlock = blockCounter.find(curBlockID);
    if (itBlock == blockCounter.end()) {
        blockCounter.emplace(curBlockID, 1);
    } else {
        blockCounter[curBlockID] = itBlock->second + 1;
    }

#ifdef COLLECT_BRANCH_COVERAGE
    BranchID curBranchID = GetBranchID(curBlockID, prevBlockID);
    auto itBranch = branchCounter.find(curBranchID);
    if (itBranch == branchCounter.end()) {
        branchCounter.emplace(curBranchID, 1);
    } else {
        branchCounter[curBranchID] = itBranch->second + 1;
    }
    prevBlockID = curBlockID;
#endif
}

bool ThreadCoverageInfo::Serialize(rapidjson::Writer<rapidjson::StringBuffer> *writer) const {
    writer->StartObject();

    writer->String("tid");
    writer->Int(this->tid);

    writer->String("blockCount");
    writer->Int(this->blockCounter.size());

    writer->String("blockCoverage");
    writer->StartArray();
    for (const auto &it : this->blockCounter) {
        writer->Int64(it.first);
        writer->Int(it.second);
    }
    writer->EndArray();

#ifdef COLLECT_BRANCH_COVERAGE
    writer->String("branchCount");
    writer->Int(this->branchCounter.size());

    writer->String("branchCoverage");
    writer->StartArray();
    for (const auto &it : this->branchCounter) {
        writer->Int64(it.first);
        writer->Int(it.second);
    }
    writer->EndArray();
#endif

    writer->EndObject();
    return true;
}

std::string ThreadCoverageInfo::print() const {
    int counter;
    std::ostringstream oss;
    oss << "Coverage(tid-" << tid << ") : {" << std::endl;
    if (!blockCounter.empty()) {
        counter = 0;
        oss << "block counter: [ ";
        for (auto it : blockCounter) {
            oss << "(" << it.first << ", " << it.second << ") ";
            if (++counter > PRINT_MAX_LEN) {
                break;
            }
        }
        oss << " ], " << std::endl;
    }

#ifdef COLLECT_BRANCH_COVERAGE
    if (!branchCounter.empty()) {
        counter = 0;
        oss << "branch counter: [ ";
        for (auto it : branchCounter) {
            oss << "(" << it.first << ", " << it.second << ") ";
            if (++counter > PRINT_MAX_LEN) {
                break;
            }
        }
        oss << " ]" << std::endl;
    }
#endif

    oss << "}";
    return oss.str();
}


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