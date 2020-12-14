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

extern const char *LOG_TAG;

static const std::vector<std::string> IGNORE_MODULES = {
    "libc.so", "libc++.so", "libutils.so",
    "libagent.so"
};

static FridaContext fridaContext = {
    nullptr,
    nullptr,
};

TRACKER_AGENT_END

#endif //AGENT_UTILS_H
