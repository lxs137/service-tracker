
#ifndef INTERCOM_CLIENT_OBSERVER_H
#define INTERCOM_CLIENT_OBSERVER_H

#include <string>

#include "pipe_ret_t.h"

typedef void (client_incoming_packet_func)(const char * msg, size_t size);
typedef client_incoming_packet_func* client_incoming_packet_func_t;

typedef void (client_disconnected_func)(const pipe_ret_t & ret);
typedef client_disconnected_func* client_disconnected_func_t;

struct client_observer_t {

    std::string wantedIp;
    client_incoming_packet_func_t incoming_packet_func;
    client_disconnected_func_t disconnected_func;

    client_observer_t() {
        wantedIp = "";
        incoming_packet_func = NULL;
        disconnected_func = NULL;
    }
};


#endif //INTERCOM_CLIENT_OBSERVER_H
