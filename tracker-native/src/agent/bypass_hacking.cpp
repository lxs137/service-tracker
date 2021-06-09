//
// Created by liao xiangsen on 3/25/21.
//

#include <frida-core.h>
#include <frida-gum.h>
#include <gumpp.hpp>

#include "agent/utils.h"
#include "base/utils.h"
#include "agent/bypass_hacking.h"

TRACKER_AGENT_USING
using ::LOG_TAG;

#define FUZZER_UID 10127
#define ANDROID_SYSTEM_UID 1000

void BypassPermissionChecking::on_enter(Gum::InvocationContext *context) {}

// All Binder call from Fuzzer looks like from System
void BypassPermissionChecking::on_leave(Gum::InvocationContext *context) {
    int uid = GPOINTER_TO_INT(context->get_return_value_ptr());
    if (uid == FUZZER_UID) {
        LOG_INFO("convert fuzzer UID(%d) to system_uid(%d)", uid, ANDROID_SYSTEM_UID);
        gum_cpu_context_replace_return_value(
            reinterpret_cast<GumCpuContext*>(context->get_cpu_context()), GINT_TO_POINTER(ANDROID_SYSTEM_UID));
    }
}

extern "C"
void do_bypass_hacking(const gchar *data, gboolean *stay_resident) {
    // keep injected library when frida injector disconnect
    *stay_resident = TRUE;
    LOG_INFO("hacking start");

    gum_init_embedded();

    const char *moduleName = "libbinder.so";
    const char *funcSymbol = "_ZNK7android14IPCThreadState13getCallingUidEv";

    gpointer funcAddress = GUINT_TO_POINTER(gum_module_find_export_by_name(moduleName, funcSymbol));
    if (funcAddress == nullptr) {
        LOG_ERROR("Function(%s) not exist in %s", funcSymbol, moduleName);
        return;
    }

    LOG_INFO("intercept function(%p)", funcAddress);
    auto interceptor = Gum::Interceptor_obtain();
    interceptor->begin_transaction();
    interceptor->attach(funcAddress, new BypassPermissionChecking, nullptr);
    interceptor->end_transaction();
}