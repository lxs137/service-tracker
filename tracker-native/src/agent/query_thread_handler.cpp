//
// Created by liao xiangsen on 12/10/20.
//

#include <fstream>

#include "base/encoder.h"
#include "agent/query_thread_handler.h"

#define THREAD_NAME_MAX_LEN 16

TRACKER_AGENT_USING
const char *QueryThreadHandler::LOG_TAG = "agent[" STRINGIFY(ARCH_ABI) "].query_thread";

void QueryThreadHandler::handle(const Client & client, const char * msg, size_t size) {
    HackAction action;
    if (parseHackAction(action, msg, size) != QueryThread) {
        return;
    }

    auto lock = std::unique_lock<std::mutex>(QueryThreadHandler::queryLock, std::defer_lock);
    LOG_INFO("handle QueryThread begin");

    // clear reply JSON
    QueryThreadHandler::threadsInfo.clear();

    gum_process_enumerate_threads(QueryThreadHandler::getThreadBacktrace, nullptr);

    LOG_INFO("parse threads detail done: %lu", QueryThreadHandler::threadsInfo.size());

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    writer.StartArray();
    for (const auto &val : QueryThreadHandler::threadsInfo) {
        val.Serialize(&writer);
    }
    writer.EndArray();

    LOG_INFO("handle QueryThread end, reply: %s", sb.GetString());
    Server::sendToClient(client, sb.GetString(), sb.GetSize());
}

gboolean QueryThreadHandler::getThreadBacktrace(const GumThreadDetails *details, gpointer user_data) {
    ThreadInfo info;
    info.tid = (int) details->id;

    LOG_INFO("parse thread[%lu] detail", details->id);

    std::string threadName;
    std::ifstream ifile(string_format("/proc/self/task/%d/comm", info.tid));
    ifile >> threadName;
    info.name = std::string(threadName);

    GumReturnAddressArray retAddrs;
    GumBacktracer *tracer = gum_backtracer_make_fuzzy();
    gum_backtracer_generate(tracer, &(details->cpu_context), &retAddrs);
    GumDebugSymbolDetails symbolDetails;
    for (int i = 0; i != retAddrs.len; i++) {
        gum_symbol_details_from_address(retAddrs.items[i], &symbolDetails);
        info.backtrace.emplace_back(symbolDetails.module_name, symbolDetails.symbol_name);
    }

    QueryThreadHandler::threadsInfo.push_back(info);
    return true;
}