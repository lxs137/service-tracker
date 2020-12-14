//
// Created by liao xiangsen on 11/23/20.
//

#ifndef AGENT_TRACE_THREAD_H
#define AGENT_TRACE_THREAD_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "tcp_client_server/server.h"
#include "base/hack_action.h"
#include "agent/utils.h"

TRACKER_AGENT_BEGIN

namespace TraceThreadHandler {
  extern const char *LOG_TAG;

  static std::mutex tracingLock;
  static std::vector<GumThreadId> followingThreads;
  // map<tid, info>
  static std::map<int, ThreadCoverageInfo> coverageInfo;

  extern void handle(const Client & client, const char * msg, size_t size);
}

static GumStalkerTransformerCallback instructionTransformer;

TRACKER_AGENT_END


//#if ARCH_ABI == arm64-v8a
//void arm64_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data);
//static GumStalkerTransformerCallback instructionTransformer = arm64_transformWhileCodeBlockEnd;
//#elif ARCH_ABI == armeabi-v7a
//void arm_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data);
//static GumStalkerTransformerCallback instructionTransformer = transformerForCodeBlock;
//#endif

#endif //AGENT_TRACE_THREAD_H
