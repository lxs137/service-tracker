//
// Created by liao xiangsen on 12/10/20.
//

#ifndef AGENT_QUERY_THREAD_H
#define AGENT_QUERY_THREAD_H

#include <iostream>
#include <mutex>
#include <vector>
#include <map>

#include "tcp_client_server/server.h"
#include "base/hack_action.h"
#include "agent/utils.h"
#include "agent/types.h"

TRACKER_AGENT_BEGIN

namespace QueryThreadHandler {
  extern const char *LOG_TAG;

  static std::mutex queryLock;
  static std::vector<ThreadInfo> threadsInfo;

  gboolean getThreadBacktrace(const GumThreadDetails *details, gpointer user_data);
  extern void handle(const Client & client, const char * msg, size_t size);
}

TRACKER_AGENT_END

#endif //AGENT_QUERY_THREAD_H
