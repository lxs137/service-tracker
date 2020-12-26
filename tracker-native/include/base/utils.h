//
// Created by liao xiangsen on 11/17/20.
//

#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <codecvt>

#include <android/log.h>

#define TRACKER_BEGIN namespace tracker {
#define TRACKER_END }
#define TRACKER_USING using namespace tracker;

#define TRACKER_AGENT_BEGIN namespace tracker { namespace agent {
#define TRACKER_AGENT_END }}
#define TRACKER_AGENT_USING using namespace tracker::agent;

#define TRACKER_INJECTOR_BEGIN namespace tracker { namespace injector {
#define TRACKER_INJECTOR_END }}
#define TRACKER_INJECTOR_USING using namespace tracker::injector;

template<typename T>
std::string join(const std::vector<T> &v, const std::string &sep) {
    if (v.empty()) {
        return "";
    }
    std::ostringstream oss;
    const auto LAST = v.end() - 1;
    // Iterate through the first to penultimate items appending the separator.
    for (typename std::vector<T>::const_iterator p = v.begin(); p != LAST; ++p) {
        oss << *p << sep;
    }
    // Join the last item without a separator.
    oss << *LAST;
    return oss.str();
}

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

std::string inline genTagByPid(const char *tag) {
    return string_format("%s[%d]", tag, getpid());
}

std::string hexDump(const uint8_t *bytes, size_t len);

std::string inline narrowChar16(const char16_t *str16) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(str16);
}

#define  LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOG_FAULT(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define  LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_DEBUG(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#endif //UTILS_H
