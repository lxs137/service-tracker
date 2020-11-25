//
// Created by liao xiangsen on 11/25/20.
//

#ifndef AGENT_UTILS_H
#define AGENT_UTILS_H

#include <string>

#include "android/log.h"

#include "utils.h"

const char *LOG_TAG = std::string("Agent(" + std::to_string(getpid()) + ")").c_str();

void inline LOG_FAULT(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, fmt, args);
    va_end(args);
}

void inline LOG_ERROR(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, args);
    va_end(args);
}

void inline LOG_INFO(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, args);
    va_end(args);
}

void inline LOG_DEBUG(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, args);
    va_end(args);
}

#endif //AGENT_UTILS_H
