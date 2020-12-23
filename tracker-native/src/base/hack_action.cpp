//
// Created by liao xiangsen on 12/10/20.
//

#include <android/log.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "tcp_client_server/server.h"
#include "base/utils.h"
#include "base/hack_action.h"

TRACKER_BEGIN

HackActionCommand parseHackAction(HackAction &hackAction, const char * msg, size_t size) {
    __android_log_print(ANDROID_LOG_INFO, "tracker",
                        "Got client msg : %s", msg);
    if (hackAction.data.Parse(msg).HasParseError()) {
        __android_log_print(ANDROID_LOG_ERROR, "tracker",
                            "invalid json: %s", rapidjson::GetParseError_En(hackAction.data.GetParseError()));
        return Invalid;
    }
    const rapidjson::Value &command = hackAction.data["command"];
    if (!command.IsNumber() || command.GetInt() >= MAX_HACK_ACTION_COMMAND) {
        return Invalid;
    }
    hackAction.command = static_cast<HackActionCommand>(command.GetInt());
    return hackAction.command;
}

void logDisconnect(const Client & client) {
    __android_log_print(ANDROID_LOG_INFO, "tracker",
                        "Client(%s:%d) disconnected", client.getIp().c_str(), client.getPort());
}

TRACKER_END