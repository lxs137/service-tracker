//
// Created by liao xiangsen on 11/16/20.
//
#ifndef INJECTOR_INJECT_AGENT_H
#define INJECTOR_INJECT_AGENT_H

#include "tcp_client_server/server.h"
#include "base/hack_action.h"

TRACKER_INJECTOR_BEGIN

namespace InjectAgentHandler {
  static std::mutex injectorLock;
  extern void handle(const Client & client, const char * msg, size_t size);
}

TRACKER_INJECTOR_END

#endif //INJECTOR_INJECT_AGENT_H
