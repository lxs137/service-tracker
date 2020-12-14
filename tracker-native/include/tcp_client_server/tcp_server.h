//
// Created by liao xiangsen on 11/25/20.
//

#ifndef INTERCOM_TCP_SERVER_H
#define INTERCOM_TCP_SERVER_H

#include "server.h"

class TCPServer : public Server {
private:
  int port;
public:
  TCPServer(int _port): Server() {
    this->port = _port;
  }
  pipe_ret_t startServer() {
      pipe_ret_t ret;
      struct sockaddr_in addr;
      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      addr.sin_port = htons(this->port);

      m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (m_sockfd < 0) {
          goto error;
      }
      if (bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
          goto error;
      }
      if (listen(m_sockfd, LISTEN_CLIENTS_QUEUE_SIZE) < 0) {
          goto error;
      }
      ret.success = true;
      return ret;

      error:
      ret.success = false;
      ret.msg = strerror(errno);
      return ret;
  }
};

#endif //INTERCOM_TCP_SERVER_H
