//
// Created by erauper on 4/7/19.
//

#ifndef INTERCOM_TCP_CLIENT_MANAGER_H
#define INTERCOM_TCP_CLIENT_MANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "client_manager.h"

class TcpClientManager : public ClientManager {
public:
  pipe_ret_t connectTo(const char *address, int port) {
      pipe_ret_t ret;
      struct sockaddr_in m_server;
      m_sockfd = 0;

      m_sockfd = socket(AF_INET , SOCK_STREAM , 0);
      if (m_sockfd == -1) { //socket failed
          ret.success = false;
          ret.msg = strerror(errno);
          return ret;
      }

      int inetSuccess = inet_aton(address, &m_server.sin_addr);

      if(!inetSuccess) { // inet_addr failed to parse address
          // if hostname is not in IP strings and dots format, try resolve it
          struct hostent *host;
          struct in_addr **addrList;
          if ((host = gethostbyname(address)) == nullptr){
              ret.success = false;
              ret.msg = "Failed to resolve hostname";
              return ret;
          }
          addrList = (struct in_addr **) host->h_addr_list;
          m_server.sin_addr = *addrList[0];
      }
      m_server.sin_family = AF_INET;
      m_server.sin_port = htons( port );

      int connectRet = connect(m_sockfd , (struct sockaddr *)&m_server , sizeof(m_server));
      if (connectRet == -1) {
          ret.success = false;
          ret.msg = strerror(errno);
          return ret;
      }
      m_receiveTask = new std::thread(&TcpClientManager::ReceiveTask, this);
      ret.success = true;
      return ret;
  }
};

#endif //INTERCOM_TCP_CLIENT_MANAGER_H
