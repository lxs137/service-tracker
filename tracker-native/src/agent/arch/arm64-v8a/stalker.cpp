//
// Created by liao xiangsen on 12/10/20.
//

#include <frida-gum.h>

#include "agent/utils.h"
#include "agent/trace_thread_handler.h"

void arm64_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data);
void arm64_trackCodeBlockCoverage(GumCpuContext *cpu_context, gpointer user_data);

TRACKER_AGENT_USING
using ::LOG_TAG;
GumStalkerTransformerCallback TraceThreadHandler::transformerPtr = &arm64_transformWhileCodeBlockEnd;

void arm64_trackCodeBlockCoverage(GumCpuContext *cpu_context, gpointer user_data) {
    void *ptr = pthread_getspecific(fridaContext->keyToCoverageInfo);
    assert(ptr != nullptr);
    auto infoPtr = static_cast<ThreadCoverageInfo*>(ptr);
    infoPtr->incCounter((BlockID) user_data);
//    LOG_INFO("PC at %p, block counter: %d", user_data, infoPtr->blockCounter.size());
}

void arm64_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data) {
    void *ptr = pthread_getspecific(fridaContext->keyToCoverageInfo);
    if (ptr == nullptr) {
        auto lock = std::unique_lock<std::mutex>(TraceThreadHandler::coverageLock, std::defer_lock);
        int tid = gum_process_get_current_thread_id();
        auto infoPtr = new ThreadCoverageInfo(tid);
        pthread_setspecific(fridaContext->keyToCoverageInfo, infoPtr);
        TraceThreadHandler::coverageInfoMap[tid] = infoPtr;
    }

    const cs_insn *instructionInfo;
    while (gum_stalker_iterator_next (iterator, &instructionInfo)) {
        switch (instructionInfo->id) {
            /**
             * Instruction list is from
             * frida-gum/gum/backend-arm64/gumstalker-arm64.c:gum_stalker_iterator_keep()
             */
            case ARM64_INS_B:
            case ARM64_INS_BL:
            case ARM64_INS_BLR:
            case ARM64_INS_BLRAA:
            case ARM64_INS_BLRAAZ:
            case ARM64_INS_BLRAB:
            case ARM64_INS_BLRABZ: {

                gpointer blockAddress = GUINT_TO_POINTER(instructionInfo->address);
                auto findModule = gum_module_map_find(fridaContext->moduleMap, GUM_ADDRESS(blockAddress));
                if (findModule != nullptr) {
                    LOG_INFO("Transform inst in module %s at %p", findModule->name, blockAddress);
                    gpointer shortAddress = GUINT_TO_POINTER(
                        GUM_ADDRESS(g_str_hash(findModule->name) << 32)
                        ^ ((GUM_ADDRESS(blockAddress) - findModule->range->base_address) >> 1)
                    );
//                    gpointer shortAddress = GUINT_TO_POINTER(instructionInfo->address >> 1);
                    gum_stalker_iterator_put_callout(iterator, arm64_trackCodeBlockCoverage, shortAddress, nullptr);
                }
            }
        }
        gum_stalker_iterator_keep(iterator);
    }
}
