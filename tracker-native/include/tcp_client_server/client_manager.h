//
// Created by liao xiangsen on 12/11/20.
//

#ifndef INTERCOM_CLIENT_MANAGER_H
#define INTERCOM_CLIENT_MANAGER_H

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <thread>

#include "client_observer.h"
#include "pipe_ret_t.h"

#define MAX_PACKET_SIZE 20480

class ClientManager {
protected:
  int m_sockfd = 0;
  bool stop = false;
  std::vector<client_observer_t> m_subscibers;
  std::thread *m_receiveTask = nullptr;

  void publishServerMsg(const char *msg, size_t msgSize);
  void publishServerDisconnected(const pipe_ret_t & ret);
  void terminateReceiveThread();
  void ReceiveTask();
public:
  virtual ~ClientManager();
  pipe_ret_t sendMsg(const char *msg, size_t size);
  void subscribe(const client_observer_t &observer);
  void unsubscribeAll();
  void waitResponse();
  pipe_ret_t finish();
};

#endif //INTERCOM_CLIENT_MANAGER_H
