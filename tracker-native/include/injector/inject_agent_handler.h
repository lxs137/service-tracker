//
// Created by liao xiangsen on 11/16/20.
//
#ifndef INJECTOR_INJECT_AGENT_H
#define INJECTOR_INJECT_AGENT_H

#include "tcp_client_server/tcp_server.h"
#include "hack_action.h"

class InjectAgentHandler : HackActionHandler {
public:
    static void handle(const Client & client, const char * msg, size_t size);
    static void disconnect(const Client & client) { logDisconnect(client); }
};

#endif //INJECTOR_INJECT_AGENT_H
