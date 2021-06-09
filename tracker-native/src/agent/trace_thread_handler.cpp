//
// Created by liao xiangsen on 11/23/20.
//

#include <frida-gum.h>

#include "agent/trace_thread_handler.h"
#include "agent/utils.h"
#include "base/utils.h"

TRACKER_AGENT_USING
const char *TraceThreadHandler::LOG_TAG = "tracker.agent[" STRINGIFY(ARCH_ABI) "].trace_thread";
std::map<GumThreadId, ThreadCoverageInfo*> TraceThreadHandler::coverageInfoMap;
std::mutex TraceThreadHandler::tracingLock;
std::mutex TraceThreadHandler::coverageLock;

void TraceThreadHandler::handle(const Client &client, const char *msg, size_t size) {
    HackAction action;
    switch (parseHackAction(action, msg, size)) {
        case TraceThreadCoverage: {
            auto lock = std::unique_lock<std::mutex>(tracingLock, std::defer_lock);
            LOG_INFO("handle TraceThread begin");

            const rapidjson::Value &tids = action.data["tids"].GetArray();
            std::vector<GumThreadId> needFollowThreads;
            for (rapidjson::SizeType i = 0; i < tids.Size(); i++) {
                auto tid = (GumThreadId) tids[i].GetInt();
                if (std::find(followingThreads.begin(), followingThreads.end(), tid) == followingThreads.end()) {
                    needFollowThreads.push_back(tid);
                }
            }

            LOG_INFO("Stalker(%p) try to stalk threads: [ %s ]", fridaContext->stalker, join(needFollowThreads, ",").c_str());

            /**
             * Stalker generates events when a followed thread is being executed,
             * EventSink is used to tell Stalker how to flush these events to user program(Node.js or Python).
             * Usage guide is at: https://github.com/DefinitelyTyped/DefinitelyTyped/blob/master/types/frida-gum/index.d.ts#StalkerOptions
             */
            GumEventSink *dummyEventSink = gum_event_sink_make_default();
            for (auto tid : needFollowThreads) {
                GumStalkerTransformer *transformer = gum_stalker_transformer_make_from_callback(
                    transformerPtr, nullptr, nullptr);
                gum_stalker_follow(fridaContext->stalker, tid, transformer, dummyEventSink);
                followingThreads.push_back(tid);
            }

            const char *replyStr = basicReply(true);
            Server::sendToClient(client, replyStr, strlen(replyStr));
        } break;
        case QueryCoverageInfo: {
            const rapidjson::Value &tids = action.data["tids"].GetArray();
            rapidjson::StringBuffer sb;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

            writer.StartArray();
            for (rapidjson::SizeType i = 0; i < tids.Size(); i++) {
                auto tid = (GumThreadId) tids[i].GetInt();
                auto infoIt = coverageInfoMap.find(tid);
                if (infoIt == coverageInfoMap.end()) {
                    continue;
                }
                infoIt->second->Serialize(&writer);
            }
            writer.EndArray();
            Server::sendToClient(client, sb.GetString(), sb.GetSize());
        } break;
        case ResetCoverageInfo: {
            auto lock = std::unique_lock<std::mutex>(TraceThreadHandler::coverageLock, std::defer_lock);
            for (auto &infoIt : coverageInfoMap) {
                infoIt.second->reset();
            }
            const char *replyStr = basicReply(true);
            Server::sendToClient(client, replyStr, strlen(replyStr));
        } break;
        default: return;
    }
}

