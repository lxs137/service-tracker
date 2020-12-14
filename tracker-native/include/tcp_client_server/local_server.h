//
// Created by liao xiangsen on 11/25/20.
//

#ifndef INTERCOM_LOCAL_SERVER_H
#define INTERCOM_LOCAL_SERVER_H

#include <sys/un.h>

#include "server.h"

class LocalServer : public Server {
private:
  const char *unixDomainSocket;
public:
  LocalServer(const char *udsPath): Server() {
      this->unixDomainSocket = udsPath;
  }
  pipe_ret_t startServer() {
      pipe_ret_t ret;
      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strcpy(addr.sun_path, this->unixDomainSocket);
      int addrLength = offsetof(struct sockaddr_un, sun_path) + strlen(this->unixDomainSocket);

      m_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (m_sockfd < 0) {
          goto error;
      }
      unlink(this->unixDomainSocket);
      if (bind(m_sockfd, (struct sockaddr*)&addr, addrLength) < 0) {
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

#endif //INTERCOM_LOCAL_SERVER_H
