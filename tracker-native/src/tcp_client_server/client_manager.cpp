//
// Created by liao xiangsen on 12/11/20.
//

#include <sys/socket.h>

#include "tcp_client_server/client_manager.h"

pipe_ret_t ClientManager::sendMsg(const char * msg, size_t size) {
    pipe_ret_t ret;
    int numBytesSent = send(m_sockfd, msg, size, 0);
    if (numBytesSent < 0 ) { // send failed
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

void ClientManager::subscribe(const client_observer_t & observer) {
    m_subscibers.push_back(observer);
}

void ClientManager::unsubscribeAll() {
    m_subscibers.clear();
}

/*
 * Publish incoming client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void ClientManager::publishServerMsg(const char * msg, size_t msgSize) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].incoming_packet_func != NULL) {
            (*m_subscibers[i].incoming_packet_func)(msg, msgSize);
        }
    }
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void ClientManager::publishServerDisconnected(const pipe_ret_t & ret) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].disconnected_func != NULL) {
            (*m_subscibers[i].disconnected_func)(ret);
        }
    }
}

/*
 * Receive server packets, and notify user
 */
void ClientManager::ReceiveTask() {

    while(!stop) {
        char msg[MAX_PACKET_SIZE];
        int numOfBytesReceived = recv(m_sockfd, msg, MAX_PACKET_SIZE, 0);
        if(numOfBytesReceived < 1) {
            pipe_ret_t ret;
            ret.success = false;
            stop = true;
            if (numOfBytesReceived == 0) { //server closed connection
                ret.msg = "Server closed connection";
            } else {
                ret.msg = strerror(errno);
            }
            publishServerDisconnected(ret);
            finish();
            break;
        } else {
            publishServerMsg(msg, numOfBytesReceived);
        }
    }
}

pipe_ret_t ClientManager::finish(){
    stop = true;
    terminateReceiveThread();
    pipe_ret_t ret;
    if (close(m_sockfd) == -1) { // close failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    ret.success = true;
    return ret;
}

void ClientManager::terminateReceiveThread() {
    if (m_receiveTask != nullptr && !m_receiveTask->joinable()) {
        m_receiveTask->detach();
        delete m_receiveTask;
        m_receiveTask = nullptr;
    }
}

ClientManager::~ClientManager() {
    terminateReceiveThread();
}

void ClientManager::waitResponse() {
    if (m_receiveTask != nullptr) {
        m_receiveTask->join();
    }
}
