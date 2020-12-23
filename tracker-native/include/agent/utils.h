//
// Created by liao xiangsen on 11/25/20.
//

#ifndef AGENT_UTILS_H
#define AGENT_UTILS_H

#include <iostream>
#include <map>

#include "frida-gum.h"

#include "base/utils.h"
#include "agent/types.h"

TRACKER_AGENT_BEGIN

static const char *LOG_TAG = "tracker.agent[" STRINGIFY(ARCH_ABI) "]";

static const std::vector<std::string> IGNORE_MODULES = {
    "libc.so", "libc++.so", "libutils.so", "liblog.so",
    "libagent.so"
};
//static const std::vector<std::string> IGNORE_MODULES = { "libagent.so" };

extern FridaContext *fridaContext;

TRACKER_AGENT_END

#endif //AGENT_UTILS_H
