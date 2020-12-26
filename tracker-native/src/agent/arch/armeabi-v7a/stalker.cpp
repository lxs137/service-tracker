//
// Created by liao xiangsen on 12/10/20.
//

#include <iostream>
#include <map>
#include <mutex>

#include <frida-gum.h>

#include "agent/utils.h"
#include "agent/trace_thread_handler.h"

void arm_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data);
void arm_trackCodeBlockCoverage(GumCpuContext *cpu_context, gpointer user_data);

TRACKER_AGENT_USING
using ::LOG_TAG;
GumStalkerTransformerCallback TraceThreadHandler::transformerPtr = &arm_transformWhileCodeBlockEnd;
thread_local TransformerContext TraceThreadHandler::transformerContext;

void arm_trackCodeBlockCoverage(GumCpuContext *cpu_context, gpointer user_data) {
    ThreadCoverageInfo *info = TraceThreadHandler::transformerContext.coverageInfoPtr;
    info->incCounter((BlockID) user_data);
    LOG_INFO("PC at %p, block counter: %d, branch counter: %d", user_data, info->blockCounter.size(), info->branchCounter.size());
}

void arm_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data) {
    int tid = gum_process_get_current_thread_id();
    {
        auto lock = std::unique_lock<std::mutex>(TraceThreadHandler::coverageLock, std::defer_lock);
        auto it = TraceThreadHandler::coverageInfoMap.find(tid);
        if (it != TraceThreadHandler::coverageInfoMap.end()) {
            TraceThreadHandler::transformerContext.coverageInfoPtr = it->second;
        } else {
            ThreadCoverageInfo *infoPtr = new ThreadCoverageInfo(tid);
            TraceThreadHandler::transformerContext.coverageInfoPtr = infoPtr;
            TraceThreadHandler::coverageInfoMap[tid] = infoPtr;
        }
    }

    const cs_insn *instructionInfo;
    while (gum_stalker_iterator_next(iterator, &instructionInfo)) {
        switch (instructionInfo->id) {
            /**
             * Instruction list is from
             * https://developer.arm.com/documentation/dui0489/i/arm-and-thumb-instructions/branch-and-control-instructions
             */
            case ARM_INS_B:
            case ARM_INS_BL:
            case ARM_INS_BX:
            case ARM_INS_BLX:
            case ARM_INS_BXJ:
            case ARM_INS_IT:
            // switch-case assembly
            case ARM_INS_TBB:
            case ARM_INS_TBH:
            // compare and branch
            case ARM_INS_CBZ:
            case ARM_INS_CBNZ: {

                gpointer blockAddress = GUINT_TO_POINTER(instructionInfo->address);
                auto findModule = gum_module_map_find(fridaContext->moduleMap, GUM_ADDRESS(blockAddress));
                if (findModule != nullptr) {
                    LOG_INFO("Transform inst in module %s at %p", findModule->name, blockAddress);
//                    gpointer shortAddress = GUINT_TO_POINTER(
//                        GUM_ADDRESS(g_str_hash(findModule->name))
//                        ^ ((GUM_ADDRESS(blockAddress) - findModule->range->base_address) >> 1)
//                    );
                    gpointer shortAddress = GUINT_TO_POINTER(instructionInfo->address >> 1);
                    gum_stalker_iterator_put_callout(iterator, arm_trackCodeBlockCoverage, shortAddress, nullptr);
                }
            }
        }
        gum_stalker_iterator_keep(iterator);
    }
}
