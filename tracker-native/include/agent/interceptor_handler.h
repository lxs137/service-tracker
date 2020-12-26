//
// Created by liao xiangsen on 12/23/20.
//

#ifndef AGENT_INTERCEPTOR_H
#define AGENT_INTERCEPTOR_H

#include <iostream>
#include <mutex>
#include <set>

#include "tcp_client_server/server.h"
#include "base/hack_action.h"
#include "agent/utils.h"

TRACKER_AGENT_BEGIN

namespace InterceptorHandler {
  class LoggingIoctl: public Gum::InvocationListener {
  private:
    bool skip = false;
    void *customDataPtr = nullptr;
  public:
    void on_enter (Gum::InvocationContext *context) override;
    void on_leave (Gum::InvocationContext *context) override;
  };

  extern const char *LOG_TAG;

  extern std::mutex interceptLock;
  extern std::map<uintptr_t, Gum::InvocationListener*> activeListener;

  extern void handle(const Client &client, const char *msg, size_t size);
}

TRACKER_AGENT_END

#endif //AGENT_INTERCEPTOR_H
