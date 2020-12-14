

#ifndef INTERCOM_SERVER_OBSERVER_H
#define INTERCOM_SERVER_OBSERVER_H

#include <string>
#include "client.h"

typedef void (server_incoming_packet_func)(const Client & client, const char * msg, size_t size);
typedef server_incoming_packet_func* server_incoming_packet_func_t;

typedef void (server_disconnected_func)(const Client & client);
typedef server_disconnected_func* server_disconnected_func_t;

struct server_observer_t {

	std::string wantedIp;
	server_incoming_packet_func_t incoming_packet_func;
	server_disconnected_func_t disconnected_func;

	server_observer_t() {
		wantedIp = "";
		incoming_packet_func = NULL;
		disconnected_func = NULL;
	}
};

#endif //INTERCOM_SERVER_OBSERVER_H
