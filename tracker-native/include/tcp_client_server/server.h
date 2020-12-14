

#ifndef INTERCOM_SERVER_H
#define INTERCOM_SERVER_H



#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>

#include "client.h"
#include "server.h"
#include "server_observer.h"
#include "pipe_ret_t.h"


#define MAX_PACKET_SIZE 20480
#define LISTEN_CLIENTS_QUEUE_SIZE 5

class Server {
protected:
  int m_sockfd;
  std::vector<Client> m_clients;
  std::vector<server_observer_t> m_subscibers;

  void publishClientMsg(const Client &client, const char *msg, size_t msgSize);
  void publishClientDisconnected(const Client &client);
  void receiveTask(/*void * context*/);
public:
  Server() {
      m_sockfd = 0;
      m_clients.reserve(10);
      m_subscibers.reserve(10);
  }
  virtual pipe_ret_t startServer() = 0;
  Client acceptClient(uint timeout_sec);
  bool deleteClient(Client &client);
  void subscribe(const server_observer_t &observer);
  void unsubscribeAll();
  pipe_ret_t sendToAllClients(const char *msg, size_t size);
  static pipe_ret_t sendToClient(const Client &client, const char *msg, size_t size);
  pipe_ret_t finish();
  std::string getInternalStats() const ;
};



#endif //INTERCOM_SERVER_H
