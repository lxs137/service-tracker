#include <string>
#include <sys/prctl.h>
#include <fcntl.h>
#include <linux/random.h>
#include <codecvt>

#include <frida-gum.h>
#include <gumpp.hpp>

#include "tcp_client_server/local_server.h"
#include "tcp_client_server/local_client_manager.h"
#include "agent/utils.h"
#include "agent/trace_thread_handler.h"
#include "agent/query_thread_handler.h"
#include "agent/interceptor_handler.h"
#include "base/utils.h"

TRACKER_AGENT_USING
using ::LOG_TAG;

FridaContext *tracker::agent::fridaContext = new FridaContext;

// we don't care code in some modules
static gboolean excludeModule(const GumModuleDetails *details, gpointer user_data) {
    for (auto &module : IGNORE_MODULES) {
        if (g_strcmp0(details->name, module.c_str()) == 0) {
            LOG_INFO("Exclude module %s %s %p-%p", details->name, details->path,
                     (void *)details->range->base_address, (void *)(details->range->base_address + details->range->size));
            gum_stalker_exclude(fridaContext->stalker, details->range);
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

    std::string cmdLine("ulimit -s unlimited; ulimit -s");

    FILE *cmd = popen(cmdLine.c_str(), "r");
    gchar *line = (gchar *) g_malloc0(50);
    fgets(line, 50, cmd);
    pclose(cmd);

    LOG_INFO("ulimit %s", line);
}

void initFridaContext() {
    gum_init_embedded();

    fridaContext->stalker = gum_stalker_new();
    /**
     * `trust_threshold` meaning:
     * The idea is that we keep checking whether a block changed every time we’re about to execute it,
     * and once it’s been successfully recycled trust-threshold times we consider it trusted,
     * and backpatch static branches with a branch straight to the recompiled target.
     */
    gum_stalker_set_trust_threshold(fridaContext->stalker, 2);

    gum_process_enumerate_modules(excludeModule, nullptr);

    fridaContext->moduleMap = gum_module_map_new_filtered(excludeModule, nullptr, nullptr);
    gum_module_map_update(fridaContext->moduleMap);

    GArray *modules = gum_module_map_get_values(fridaContext->moduleMap);
    LOG_INFO("modules count: %d", modules->len);

    fridaContext->interceptor = Gum::Interceptor_obtain();

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

    server_observer_t observerDoTrace;
    observerDoTrace.incoming_packet_func = TraceThreadHandler::handle;
    observerDoTrace.disconnected_func = tracker::logDisconnect;
    observerDoTrace.wantedIp = "";
    agentServer.subscribe(observerDoTrace);

    server_observer_t observerDoIntercept;
    observerDoIntercept.incoming_packet_func = InterceptorHandler::handle;
    observerDoIntercept.disconnected_func = tracker::logDisconnect;
    observerDoIntercept.wantedIp = "";
    agentServer.subscribe(observerDoIntercept);

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

void dummy_1() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        int fd = open("/dev/random", O_RDWR);
        if (fd < 0) {
            LOG_INFO("dummy thread-1 open error");
            continue;
        }
        int *ent_count = new int(-1);
        ioctl(fd, RNDGETENTCNT, ent_count);
        LOG_INFO("dummy thread-1 ENT_COUNT: %d", *ent_count);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        delete ent_count;
        close(fd);
    }
}

void dummy_2() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        LOG_INFO("dummy thread-2 sleeping......");
    }
}

int main(int argc, char *argv[]) {
//    std::thread dummyThread1(dummy_1), dummyThread2(dummy_2);
//    std::thread dummyThread1(dummy_1);

    int pid = getpid();
    LOG_INFO("tester server pid: %d", pid);

//    checkProcess();
//    initFridaContext();

//    std::this_thread::sleep_for(std::chrono::seconds(8));

//    std::string msg(string_format(R"({ "command": 3, "tids": [%d, %d] })", pid + 1, pid + 2));
    char16_t token[4];
    token[0] = 'a';
    token[1] = 'n';
    token[2] = 'd';
    token[3] = 0;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string a = convert.to_bytes(token);
//    char tokenReal[4];
//    narrowChar(const_cast<char16_t *>(token), tokenReal, 4);
    LOG_INFO("wchar: %s", a.c_str());
//    std::string msg(R"({ "command": 6, "moduleName": "libcutils.so", "funcSymbol": "ioctl" })");
//    Client dummyClient;
//    InterceptorHandler::handle(dummyClient, msg.c_str(), msg.size());

//    std::this_thread::sleep_for(std::chrono::seconds(60));

//    gboolean keepResident;
//    agent_server("/data/local/tmp/tracker/sockets/agent.socket", &keepResident);

//    TcpClientManager tcpClient;
//    tcpClient.connectTo("127.0.0.1", 4444);

//    LocalClientManager localClient;
//    localClient.connectTo("/data/local/tmp/tracker/sockets/agent.socket");
}

