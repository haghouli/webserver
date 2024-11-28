#ifndef MULTI_HPP
#define MULTI_HPP

#include "../../File_parser/includes/response.hpp"
#include "../../Methodes/includes/Post.hpp"
#include "../../Methodes/includes/delete.hpp"

void                            startTheServer(Data *data);
int                             parseRequestAndHeaders(Client* client);
void                            addSocketToEpoll(int epoll_fd, int sockfd);
void                            setNonBlocking(int sockfd);
uint16_t                        getPort(const std::string &port);
uint32_t                        getIp(const std::string &ip);
std::vector<int>::iterator      easyfind(std::vector<int> &ar, int elementFind);
void                            dropClient(int epoll_fd, int socket, Data *data);
bool                            checkForTimeOut(Client *client, int epoll_fd, int socket, Data *data);
 
#endif