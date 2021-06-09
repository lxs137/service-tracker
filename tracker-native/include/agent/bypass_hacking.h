//
// Created by liao xiangsen on 3/25/21.
//

#ifndef AGENT_BYPASS_PERMISSION_CHECKING_H
#define AGENT_BYPASS_PERMISSION_CHECKING_H

#include <iostream>
#include <mutex>
#include <set>

#include "tcp_client_server/server.h"
#include "base/hack_action.h"
#include "agent/utils.h"

TRACKER_AGENT_BEGIN

class BypassPermissionChecking: public Gum::InvocationListener {
public:
  void on_enter (Gum::InvocationContext *context) override;
  void on_leave (Gum::InvocationContext *context) override;
};

TRACKER_AGENT_END

#endif //AGENT_BYPASS_PERMISSION_CHECKING_H
