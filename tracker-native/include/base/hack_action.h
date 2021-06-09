//
// Created by liao xiangsen on 11/17/20.
//

#ifndef HACK_ACTION_H
#define HACK_ACTION_H

#include <android/log.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "tcp_client_server/server.h"
#include "base/utils.h"

TRACKER_BEGIN

enum HackActionCommand {
  Invalid = 0,

  InjectAgent = 1,

  QueryThread = 2,

  TraceThreadCoverage = 3,
  QueryCoverageInfo = 4,
  ResetCoverageInfo = 5,

  AttachInterceptor = 6,
  QueryInterceptor = 7,

  MAX_HACK_ACTION_COMMAND,
};

struct HackAction {
  HackActionCommand command;
  rapidjson::Document data;
};

HackActionCommand parseHackAction(HackAction &hackAction, const char * msg, size_t size);

void logDisconnect(const Client & client);

TRACKER_END

#endif //HACK_ACTION_H
