#include "../includes/Multi.hpp"

struct timeval currentTime() {
    struct timeval  currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime;  
}

bool checkForTimeOut(Client *client, int epoll_fd, int socket, Data *data)
{
    if (!client->Request)
    {
        long diffSeconds = currentTime().tv_sec - client->startTime.tv_sec;
        if (diffSeconds >= 40)
        {   
            dropClient(epoll_fd, socket, data);
            return false;
        }
    }
    else if (client->request["method"] == "POST" && !client->Respond)
    {
        long diffSeconds = currentTime().tv_sec - client->postStartTime.tv_sec;
         if (diffSeconds >= 10)
        {   
            client->Respond = true;
            client->res.code = 408;
            ResponseGenerator::getResponse(client);
            return true;
        }
    }
    return true;
}

void dropClient(int epoll_fd, int socket, Data *data)
{
    std::map<int, Client*>::iterator it = data->getClientMap().lower_bound(socket);
    if (it == data->getClientMap().end() || it->first != socket)
        return;
    if(data->getClientMap()[socket]->isCgiInProcess) {
        kill(data->getClientMap()[socket]->cgiProcessId, SIGKILL);
        waitpid(data->getClientMap()[socket]->cgiProcessId, NULL, 0);
        if(access(data->getClientMap()[socket]->cgiOutputFile.c_str(), F_OK) == 0) {
            std::remove(data->getClientMap()[socket]->cgiOutputFile.c_str());
            if(data->getClientMap()[socket]->c_location->locationFile.is_open())
                data->getClientMap()[socket]->c_location->locationFile.close();
        }
        if(data->getClientMap()[socket]->hasCgi && access(data->getClientMap()[socket]->postFile.c_str(), F_OK) == 0) {
            std::remove(data->getClientMap()[socket]->postFile.c_str());
            if(data->getClientMap()[socket]->objFile.is_open())
                data->getClientMap()[socket]->objFile.close();
        }
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket, NULL);
    close(socket);
    delete data->getClientMap()[socket];
    data->getClientMap().erase(socket);
}

std::vector<int>::iterator easyfind(std::vector<int> &ar, int elementFind)
{
    return (std::find(ar.begin(), ar.end(), elementFind));
}

void isValidateIPv4(const std::string& ip) {
    std::stringstream ss(ip);
    std::string part;
    int num;

    while (std::getline(ss, part, '.')) {
        for (size_t i = 0; i < part.size(); ++i) {
            if (!isdigit(part[i])) {
                throw (std::runtime_error("IPv4 Not Validate: Forbidden_Character"));
            }
        }
        num = atoi(part.c_str());
        if (num < 0 || num > 255) {
            throw (std::runtime_error("Error: IPv4 Not Validate: Out_of_Range"));
        }
        if (part.size() > 4) {
            throw (std::runtime_error("Error: IPv4 Not Validate: Wrong_Size"));
        }
    }
}

uint32_t getIp(const std::string &ip)
{
    isValidateIPv4(ip);
    std::vector<int> ipParts;
    std::istringstream ss(ip);
    std::string part;

    while (std::getline(ss, part, '.'))
        ipParts.push_back(std::atoi(part.c_str()));
    if (ipParts.size() != 4) 
        throw (std::runtime_error("Error: invalid IP address format"));
    return static_cast<uint32_t>((ipParts[3] << 24) | (ipParts[2] << 16) | (ipParts[1] << 8) | ipParts[0]);
}

uint16_t getPort(const std::string &port)
{
    return std::atoi(port.c_str());
}

void addSocketToEpoll(int epoll_fd, int sockfd) 
{
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    event.data.fd = sockfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
        throw(std::runtime_error("Error: Failed Adding Server Socket To Epoll Instance"));
}