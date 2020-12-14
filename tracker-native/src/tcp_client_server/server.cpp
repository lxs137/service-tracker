//
// Created by liao xiangsen on 11/25/20.
//

#include <sstream>
#include <numeric>

#include "tcp_client_server/server.h"

void Server::subscribe(const server_observer_t & observer) {
    m_subscibers.push_back(observer);
}

void Server::unsubscribeAll() {
    m_subscibers.clear();
}

std::string Server::getInternalStats() const {
    int connectedClients = 0;
    for (auto it = m_clients.begin(); it != m_clients.end(); it++) {
        connectedClients += (it->isConnected() ? 1 : 0);
    }
    std::ostringstream os;
    os << "Server{" << std::endl
       << "  Clients: " << m_clients.size() << std::endl
       << "  Connected Clients: " << connectedClients << std::endl
       << "}" << std::endl;
    return os.str();
}

/*
 * Receive client packets, and notify user
 */
void Server::receiveTask() {

    Client * client = &m_clients.back();

    while(client->isConnected()) {
        char msg[MAX_PACKET_SIZE];
        int numOfBytesReceived = recv(client->getFileDescriptor(), msg, MAX_PACKET_SIZE, 0);
        if(numOfBytesReceived < 1) {
            client->setDisconnected();
            if (numOfBytesReceived == 0) {
                client->setErrorMessage("Client closed connection");
            } else {
                client->setErrorMessage(strerror(errno));
            }
            close(client->getFileDescriptor());
            publishClientDisconnected(*client);
            deleteClient(*client);
            break;
        } else {
            publishClientMsg(*client, msg, numOfBytesReceived);
        }
    }
}

/*
 * Erase client from clients vector.
 * If client isn't in the vector, return false. Return
 * true if it is.
 */
bool Server::deleteClient(Client & client) {
    int clientIndex = -1;
    for (uint i=0; i<m_clients.size(); i++) {
        if (m_clients[i] == client) {
            clientIndex = i;
            break;
        }
    }
    if (clientIndex > -1) {
        m_clients.erase(m_clients.begin() + clientIndex);
        return true;
    }
    return false;
}

/*
 * Publish incoming client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void Server::publishClientMsg(const Client & client, const char * msg, size_t msgSize) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].wantedIp == client.getIp() || m_subscibers[i].wantedIp.empty()) {
            if (m_subscibers[i].incoming_packet_func != NULL) {
                (*m_subscibers[i].incoming_packet_func)(client, msg, msgSize);
            }
        }
    }
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void Server::publishClientDisconnected(const Client & client) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].wantedIp == client.getIp()) {
            if (m_subscibers[i].disconnected_func != NULL) {
                (*m_subscibers[i].disconnected_func)(client);
            }
        }
    }
}

/*
 * Accept and handle new client socket. To handle multiple clients, user must
 * call this function in a loop to enable the acceptance of more than one.
 * If timeout argument equal 0, this function is executed in blocking mode.
 * If timeout argument is > 0 then this function is executed in non-blocking
 * mode (async) and will quit after timeout seconds if no client tried to connect.
 * Return accepted client
 */
Client Server::acceptClient(uint timeout_sec) {
    Client newClient;
    fd_set m_fds;

    if (timeout_sec > 0) {
        struct timeval tv;
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        FD_ZERO(&m_fds);
        FD_SET(m_sockfd, &m_fds);
        int selectRet = select(m_sockfd + 1, &m_fds, NULL, NULL, &tv);
        if (selectRet == -1) { // select failed
            newClient.setErrorMessage(strerror(errno));
            return newClient;
        } else if (selectRet == 0) { // timeout
            newClient.setErrorMessage("Timeout waiting for client");
            return newClient;
        } else if (!FD_ISSET(m_sockfd, &m_fds)) { // no new client
            newClient.setErrorMessage("File descriptor is not set");
            return newClient;
        }
    }

    struct sockaddr clientAddr;
    socklen_t sosize = sizeof(clientAddr);
    int file_descriptor = accept(m_sockfd, &clientAddr, &sosize);
    if (file_descriptor == -1) { // accept failed
        newClient.setErrorMessage(strerror(errno));
        return newClient;
    }

    newClient.setFileDescriptor(file_descriptor);
    newClient.setConnected();
//    newClient.setIp(inet_ntoa(m_clientAddress.sin_addr));
//    newClient.setPort(m_clientAddress.sin_port);
    m_clients.push_back(newClient);
    m_clients.back().setThreadHandler(std::bind(&Server::receiveTask, this));

    return newClient;
}

/*
 * Send message to all connected clients.
 * Return true if message was sent successfully to all clients
 */
pipe_ret_t Server::sendToAllClients(const char * msg, size_t size) {
    pipe_ret_t ret;
    for (uint i=0; i<m_clients.size(); i++) {
        ret = sendToClient(m_clients[i], msg, size);
        if (!ret.success) {
            return ret;
        }
    }
    ret.success = true;
    return ret;
}

/*
 * Return true if message was sent successfully
 */
pipe_ret_t Server::sendToClient(const Client & client, const char * msg, size_t size){
    pipe_ret_t ret;
    int numBytesSent = send(client.getFileDescriptor(), (char *)msg, size, 0);
    if (numBytesSent < 0) { // send failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    if ((uint)numBytesSent < size) { // not all bytes were sent
        ret.success = false;
        char msg[100];
        sprintf(msg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
        ret.msg = msg;
        return ret;
    }
    ret.success = true;
    return ret;
}

/*
 * Close server and clients resources.
 * Return true is success, false otherwise
 */
pipe_ret_t Server::finish() {
    pipe_ret_t ret;
    for (uint i=0; i<m_clients.size(); i++) {
        m_clients[i].setDisconnected();
        if (close(m_clients[i].getFileDescriptor()) == -1) { // close failed
            ret.success = false;
            ret.msg = strerror(errno);
            return ret;
        }
    }
    if (close(m_sockfd) == -1) { // close failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    m_clients.clear();
    ret.success = true;
    return ret;
}
