#include <string>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include "android/log.h"
#include "frida-gum.h"

#include "tcp_client_server/tcp_server.h"

#define LOG_TAG "StalkerAgent"
#define  LOG_FAULT(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define  LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOG_DEBUG(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

void initFridaStalker() {
    gum_init_embedded();

    GumStalker *stalker = gum_stalker_new();
    gum_stalker_set_trust_threshold(stalker, 3);

//    gum_process_enumerate_modules(exclude_module, NULL);

    GumModuleMap *module_map = gum_module_map_new();
    gum_module_map_update(module_map);

    /* check dumpable state */
    prctl(PR_SET_DUMPABLE, 1);
    gint result = prctl(PR_GET_DUMPABLE);
    g_assert_cmpint(result, ==, 1);
}

extern "C"
void agent_server(const gchar *data, gboolean *stay_resident) {
    // keep injected library when frida injector disconnect
    *stay_resident = TRUE;
    LOG_INFO("agent start");

    int port = 65200;
    if (strlen(data) > 0) {
        port = atoi(data);
    }
    TcpServer injectorServer;
    pipe_ret_t setupRet = injectorServer.start(port);
    if (setupRet.success) {
        LOG_INFO("Server setup succeeded");
    } else {
        LOG_ERROR("Server setup failed: %s", setupRet.msg.c_str());
        return;
    }

    while(true) {
        Client client = injectorServer.acceptClient(2);
        if (client.isConnected()) {
            LOG_INFO("Got client(%s:%d)", client.getIp().c_str(), client.getPort());
            injectorServer.printClients();
        } else {
            LOG_ERROR("Accept client failed: %s", client.getInfoMessage().c_str());
        }
    }
}


