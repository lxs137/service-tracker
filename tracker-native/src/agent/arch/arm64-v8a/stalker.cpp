//
// Created by liao xiangsen on 12/10/20.
//

#include "frida-gum.h"

#include "agent/utils.h"

TRACKER_AGENT_USING
using ::LOG_TAG;

void arm64_trackCodeBlockCoverage(GumCpuContext *cpu_context, gpointer user_data) {
//    GumThreadId curTid = gum_process_get_current_thread_id();

//    ThreadRuntimeInformation * tri;
//    guint64 n;
//    guint64 block_id, branch_id;
//
//    if (!enabled)
//        return;
//
//    tri = (ThreadRuntimeInformation *)
//        g_hash_table_lookup(thread_runtime_information_map, GSIZE_TO_POINTER(tid));
//
//    block_id = (GPOINTER_TO_UINT(user_data)>>4) ^ (GPOINTER_TO_UINT(user_data) << 8);
//
//    branch_id = tri->previous_block_id^block_id;
//
//    n = GPOINTER_TO_UINT(g_hash_table_lookup(tri->block_counters, GUINT_TO_POINTER(block_id)));
////    if (n == 0)
////        LOGD("new block id %lu", block_id);
//    g_hash_table_insert(tri->block_counters, GUINT_TO_POINTER(block_id), GUINT_TO_POINTER(n+1));
//    //LOGD("%li, %li", block_id, n);
//
//    n = GPOINTER_TO_UINT(g_hash_table_lookup(tri->branch_counters, GUINT_TO_POINTER(branch_id)));
//    g_hash_table_insert(tri->branch_counters, GUINT_TO_POINTER(branch_id), GUINT_TO_POINTER(n+1));
//    //LOGD("%li, %li", branch_id, n);
//
//    tri->previous_block_id = block_id>>1;

}

void arm64_transformWhileCodeBlockEnd(GumStalkerIterator *iterator, GumStalkerOutput *output, gpointer user_data) {
    GumModuleMap *moduleMap = (GumModuleMap *)user_data;

    const cs_insn *instructionInfo;

    while (gum_stalker_iterator_next (iterator, &instructionInfo)) {
        switch (instructionInfo->id) {
            /*
             * Code block ending instruction list is from
             * frida-gum/gum/backend-arm64/gumstalker-arm64.c:gum_stalker_iterator_keep()
             */
            case ARM64_INS_BL:
            case ARM64_INS_BLR:
            case ARM64_INS_BLRAA:
            case ARM64_INS_BLRAAZ:
            case ARM64_INS_BLRAB:
            case ARM64_INS_BLRABZ: {

                gpointer block_address = GUINT_TO_POINTER(instructionInfo->address);
                auto findModule = gum_module_map_find(moduleMap, GUM_ADDRESS(block_address));
                if (findModule != nullptr) {
                    LOG_INFO("Transforming instruction in module %s at %p", findModule->name, (void *)GUM_ADDRESS(block_address));

                    block_address =
                        GUINT_TO_POINTER(
                            (GUM_ADDRESS(g_str_hash(findModule->name)) << 32) ^
                            (GUM_ADDRESS(block_address) - findModule->range->base_address)
                        );

                    gum_stalker_iterator_put_callout(iterator, arm64_trackCodeBlockCoverage, block_address, nullptr);
                }
            }
        }
        gum_stalker_iterator_keep(iterator);
    }
}
