//
// Created by liao xiangsen on 12/10/20.
//

#ifndef INTERCOM_LOCAL_CLIENT_MANAGER_H
#define INTERCOM_LOCAL_CLIENT_MANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "client_manager.h"

class LocalClientManager : public ClientManager {
public:
  pipe_ret_t connectTo(const char *udsPath) {
      pipe_ret_t ret;

      m_sockfd = 0;
      m_sockfd = socket(AF_UNIX , SOCK_STREAM , 0);
      if (m_sockfd == -1) { //socket failed
          ret.success = false;
          ret.msg = strerror(errno);
          return ret;
      }

      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;
      strcpy(addr.sun_path, udsPath);
      int addrLength = offsetof(struct sockaddr_un, sun_path) + strlen(udsPath);

      if (connect(m_sockfd , (struct sockaddr*)&addr , addrLength) == -1) {
          ret.success = false;
          ret.msg = strerror(errno);
          return ret;
      }
      m_receiveTask = new std::thread(&LocalClientManager::ReceiveTask, this);
      ret.success = true;
      return ret;
  }
};

#endif //INTERCOM_LOCAL_CLIENT_MANAGER_H
