//
// Created by liao xiangsen on 11/14/20.
//
#include <iostream>

#include <android/log.h>
#include <frida-gum.h>

#include "tcp_client_server/tcp_server.h"
#include "injector/inject_agent_handler.h"
#include "utils.h"

#define LOG_TAG "Injector[" STRINGIFY(ARCH_ABI) "]"
#define  LOG_FAULT(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#define  LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_WARN(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

using namespace std;

// usage: ./injector [server_port]
int main(int argc, char *argv[]) {
    int port = 65100;
    if (argc > 1) {
        port = stoi(argv[1]);
    }
    TcpServer injectorServer;
    pipe_ret_t setupRet = injectorServer.start(port);
    if (setupRet.success) {
        LOG_INFO("Server setup succeeded");
    } else {
        LOG_ERROR("Server setup failed: %s", setupRet.msg.c_str());
        return EXIT_FAILURE;
    }

    server_observer_t observerDoInject;
    observerDoInject.incoming_packet_func = InjectAgentHandler::handle;
    observerDoInject.disconnected_func = InjectAgentHandler::disconnect;
    observerDoInject.wantedIp = "127.0.0.1";
    injectorServer.subscribe(observerDoInject);

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