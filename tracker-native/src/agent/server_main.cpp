#include <string>
#include <sys/prctl.h>

#include "frida-gum.h"

#include "tcp_client_server/local_server.h"
#include "tcp_client_server/local_client_manager.h"
#include "agent/utils.h"
#include "agent/query_thread_handler.h"
#include "base/utils.h"

TRACKER_AGENT_USING
const char* tracker::agent::LOG_TAG = "agent[" STRINGIFY(ARCH_ABI) "]";

// we don't care code in some modules
static gboolean excludeModule(const GumModuleDetails *details, gpointer user_data) {
    for (auto &module : IGNORE_MODULES) {
        if (g_strcmp0(details->name, module.c_str()) == 0) {
            LOG_INFO("Exclude module %s %s %p-%p", details->name, details->path,
                     (void *)details->range->base_address, (void *)(details->range->base_address + details->range->size));
            gum_stalker_exclude(fridaContext.stalker, details->range);
            return FALSE;
        }
    }
    return TRUE;
}

void checkProcess() {
    /**
     * why need `PR_SET_DUMPABLE` flag:
     * On Linux, you cannot ptrace your own process (or indeed any in the same process group),
     * so Frida creates a clone of the current process in its own process group and shares the same memory space.
     * This cloned process acts as a debugger, reading the registers of the original target process
     * and storing them in the shared memory space and then writing them back to the process on demand.
     */
    prctl(PR_SET_DUMPABLE, 1);
    gint result = prctl(PR_GET_DUMPABLE);
    g_assert_cmpint(result, ==, 1);
}

void initFridaContext() {
    gum_init_embedded();

    fridaContext.stalker = gum_stalker_new();
    /**
     * `trust_threshold` meaning:
     * The idea is that we keep checking whether a block changed every time we’re about to execute it,
     * and once it’s been successfully recycled trust-threshold times we consider it trusted,
     * and backpatch static branches with a branch straight to the recompiled target.
     */
    gum_stalker_set_trust_threshold(fridaContext.stalker, 2);

    gum_process_enumerate_modules(excludeModule, nullptr);

    fridaContext.moduleMap = gum_module_map_new_filtered(excludeModule, nullptr, nullptr);
    gum_module_map_update(fridaContext.moduleMap);

    GArray *modules = gum_module_map_get_values(fridaContext.moduleMap);
    LOG_INFO("modules count: %d", modules->len);

    LOG_INFO("stalker init done");
}

extern "C"
void agent_server(const gchar *data, gboolean *stay_resident) {
    // keep injected library when frida injector disconnect
    *stay_resident = TRUE;
    LOG_INFO("agent start");

    checkProcess();
    initFridaContext();

    LocalServer agentServer(data);
    pipe_ret_t setupRet = agentServer.startServer();
    if (setupRet.success) {
        LOG_INFO("Server setup succeeded");
    } else {
        LOG_ERROR("Server setup failed: %s", setupRet.msg.c_str());
        return;
    }

    server_observer_t observerDoQuery;
    observerDoQuery.incoming_packet_func = QueryThreadHandler::handle;
    observerDoQuery.disconnected_func = tracker::logDisconnect;
    observerDoQuery.wantedIp = "";
    agentServer.subscribe(observerDoQuery);

//    TraceThreadHandler::context = &fridaContext;
//    server_observer_t observerDoTrace;
//    observerDoTrace.incoming_packet_func = TraceThreadHandler::handle;
//    observerDoTrace.disconnected_func = TraceThreadHandler::disconnect;
//    observerDoTrace.wantedIp = "";
//    agentServer.subscribe(observerDoTrace);

    while(true) {
        Client client = agentServer.acceptClient(10);
        if (!client.isConnected()) {
            if (client.getInfoMessage().find("Timeout") != std::string::npos) {
                continue;
            }
            LOG_ERROR("Accept client failed: %s", client.getInfoMessage().c_str());
        }
    }
}


int main(int argc, char *argv[]) {
    gboolean keepResident;
    agent_server("/data/local/tmp/tracker/sockets/agent.socket", &keepResident);
//    TcpClientManager tcpClient;
//    tcpClient.connectTo("127.0.0.1", 4444);
//
//    LocalClientManager localClient;
//    localClient.connectTo("/data/local/tmp/tracker/sockets/agent.socket");
}

