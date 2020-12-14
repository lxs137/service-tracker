//
// Created by liao xiangsen on 11/23/20.
//

#include <frida-gum.h>

#include "agent/trace_thread_handler.h"
#include "agent/utils.h"
#include "base/utils.h"

TRACKER_AGENT_USING
const char *TraceThreadHandler::LOG_TAG = "agent[" STRINGIFY(ARCH_ABI) "].trace_thread";

void TraceThreadHandler::handle(const Client &client, const char *msg, size_t size) {
    HackAction action;
    if (parseHackAction(action, msg, size) != TraceThread) {
        return;
    }

    auto lock = std::unique_lock<std::mutex>(TraceThreadHandler::tracingLock, std::defer_lock);
    LOG_INFO("handle TraceThread begin");

    const rapidjson::Value &tids = action.data["tids"];
    const GumThreadId curTid = gum_process_get_current_thread_id();
    std::vector<GumThreadId> needFollowThreads;
    for (rapidjson::SizeType i = 0; i < tids.Size(); i++) {
        auto tid = (GumThreadId)tids[i].GetInt();
        if (tid == curTid) {
            continue;
        }
        auto it = std::find(followingThreads.begin(), followingThreads.end(), tid);
        if (it != followingThreads.end()) {
            needFollowThreads.push_back(tid);
        }
    }

    LOG_INFO("try to stalk threads: [ %s ]", join(followingThreads, ",").c_str());

    GumStalkerTransformer *transformer = gum_stalker_transformer_make_from_callback(
        instructionTransformer, fridaContext.moduleMap, nullptr);
    /*
     * Stalker generates events when a followed thread is being executed,
     * EventSink is used to tell Stalker how to flush these events to user program(Node.js or Python).
     * Usage guide is at: https://github.com/DefinitelyTyped/DefinitelyTyped/blob/master/types/frida-gum/index.d.ts#StalkerOptions
     */
    GumEventSink *dummyEventSink = gum_event_sink_make_default();
    for (auto tid : needFollowThreads) {
        gum_stalker_follow(fridaContext.stalker, tid, transformer, dummyEventSink);
    }
}
