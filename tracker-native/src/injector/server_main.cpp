//
// Created by liao xiangsen on 11/14/20.
//
#include <iostream>

#include <frida-core.h>
#include <frida-gum.h>

#include "tcp_client_server/tcp_server.h"
#include "injector/inject_agent_handler.h"
#include "injector/utils.h"
#include "base/utils.h"

void init_frida_context() {
    frida_init();
//    frida_selinux_patch_policy();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "usage: ./injector [listen_port]" << std::endl;
        return -1;
    }
    int port = std::stoi(argv[1]);

    init_frida_context();

    TCPServer injectorServer(port);
    pipe_ret_t setupRet = injectorServer.startServer();
    if (setupRet.success) {
        LOG_INFO("Server setup succeeded");
    } else {
        LOG_ERROR("Server setup failed: %s", setupRet.msg.c_str());
        return EXIT_FAILURE;
    }

    server_observer_t observerDoInject;
    observerDoInject.incoming_packet_func = tracker::injector::InjectAgentHandler::handle;
    observerDoInject.disconnected_func = tracker::logDisconnect;
    observerDoInject.wantedIp = "";
    injectorServer.subscribe(observerDoInject);

    while(true) {
        Client client = injectorServer.acceptClient(10);
        if (!client.isConnected()) {
            if (client.getInfoMessage().find("Timeout") != std::string::npos) {
                continue;
            }
            LOG_ERROR("Accept client failed: %s", client.getInfoMessage().c_str());
        }
    }
}