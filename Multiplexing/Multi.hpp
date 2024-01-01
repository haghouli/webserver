#ifndef MULTI_HPP
#define MULTI_HPP

#include "../File_parser/includes/utiles.hpp"

std::vector<int>::iterator easyfind(std::vector<int> &ar, int elementFind);
uint32_t                   getIp(const std::string &ip);
uint16_t                   getPort(servers_map::iterator &it);
void                       setNonBlocking(int sockfd);
int                        createAndBindSocket(servers_map::iterator &it, struct sockaddr_in &host_addr, socklen_t &addr_len);
void                       addSocketToEpoll(int epoll_fd, int sockfd);
void                       handleIncomingConnection(int epoll_fd, int serversocket, struct sockaddr_in& host_addr, socklen_t& addr_len);
void                       handleIncomingData(int socket, Data *data);
void                       handleOutgoingData(int socket, Data *data);
void                       startTheServer(Data *data);

#endif