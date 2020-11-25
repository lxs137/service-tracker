//
// Created by liao xiangsen on 11/23/20.
//

#include <frida-gum.h>

#include "agent/trace_thread_handler.h"

void TraceThreadHandler::handle(const Client &client, const char *msg, size_t size) {
    auto lock = std::unique_lock<std::mutex>(tracingLock, std::defer_lock);

}
