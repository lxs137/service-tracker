//
// Created by liao xiangsen on 11/17/20.
//

#ifndef HACK_ACTION_H
#define HACK_ACTION_H

#include <android/log.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "tcp_client_server/tcp_server.h"

enum HackActionCommand {
    InjectAgent = 0,
    MAX_HACK_ACTION_COMMAND,
};

class HackAction {
public:
    HackActionCommand command;
    rapidjson::Document data;
};

class HackActionHandler {
protected:
    static bool parseHackAction(HackAction &hackAction, const char * msg, size_t size) {
        __android_log_print(ANDROID_LOG_INFO, "Injector.HackActionHandler",
                            "Got client msg : %s", msg);
        if (hackAction.data.Parse(msg).HasParseError()) {
            __android_log_print(ANDROID_LOG_ERROR, "Injector.HackActionHandler",
                                "invalid json: %s", rapidjson::GetParseError_En(hackAction.data.GetParseError()));
            return false;
        }
        const rapidjson::Value &command = hackAction.data["command"];
        if (!command.IsNumber() || command.GetInt() >= MAX_HACK_ACTION_COMMAND) {
            return false;
        }
        hackAction.command = static_cast<HackActionCommand>(command.GetInt());
        return true;
    };
    static inline void logDisconnect(const Client & client) {
        __android_log_print(ANDROID_LOG_INFO, "Injector.HackActionHandler",
                "Client(%s:%d) disconnected", client.getIp().c_str(), client.getPort());
    }
};

#endif //HACK_ACTION_H
