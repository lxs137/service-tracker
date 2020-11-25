//
// Created by liao xiangsen on 11/23/20.
//

#ifndef AGENT_TRACE_THREAD_H
#define AGENT_TRACE_THREAD_H

#include <iostream>
#include <mutex>

#include "tcp_client_server/tcp_server.h"
#include "hack_action.h"

class TraceThreadHandler : HackActionHandler {
private:
  static std::mutex tracingLock;
  static GumStalker *stalker;
public:
  static void handle(const Client & client, const char * msg, size_t size);
  static void disconnect(const Client & client) { logDisconnect(client); }

};

#endif //AGENT_TRACE_THREAD_H
