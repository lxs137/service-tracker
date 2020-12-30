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

  extern GumStalkerTransformerCallback transformerPtr;

  extern std::mutex tracingLock, coverageLock;
  static std::vector<GumThreadId> followingThreads;

  extern std::map<GumThreadId, ThreadCoverageInfo*> coverageInfoMap;

  extern void handle(const Client & client, const char * msg, size_t size);
}

TRACKER_AGENT_END

#endif //AGENT_TRACE_THREAD_H
