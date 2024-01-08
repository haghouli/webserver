#include "Multi.hpp"

std::vector<int>::iterator easyfind(std::vector<int> &ar, int elementFind)
{
    return (std::find(ar.begin(), ar.end(), elementFind));
}

uint32_t getIp(const std::string &ip)
{
    std::vector<int> ipParts;
    std::istringstream ss(ip);
    std::string part;

    while (std::getline(ss, part, '.'))
        ipParts.push_back(std::atoi(part.c_str()));
    if (ipParts.size() != 4) 
        throw ("Invalid IP address format");
    return static_cast<uint32_t>((ipParts[3] << 24) | (ipParts[2] << 16) | (ipParts[1] << 8) | ipParts[0]);
}

uint16_t getPort(const std::string &port)
{
    return std::atoi(port.c_str());
}


void setNonBlocking(int sockfd) 
{
    int flags = fcntl(sockfd, F_GETFL);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        exit(1);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        exit(1);
    }
}

int createAndBindSocket(servers_map::iterator &it, struct sockaddr_in &host_addr, socklen_t &addr_len) 
{
    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket == -1) 
        throw ("webserver (socket)");

    setNonBlocking(serversocket);
    addr_len = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(getPort(it->first.second));
    host_addr.sin_addr.s_addr = getIp(it->first.first);
    if (bind(serversocket, (struct sockaddr *)&host_addr, addr_len) != 0)
        throw ("webserver (bind)");
    if (listen(serversocket, SOMAXCONN) != 0) 
       throw ("webserver (listen)");
    return serversocket;
}

void addSocketToEpoll(int epoll_fd, int sockfd) 
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sockfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
        throw("epoll_ctl");
}

void handleIncomingConnection(Data *data, int epoll_fd, int serversocket, struct sockaddr_in& host_addr, socklen_t& addr_len) 
{
    int clientsocket = accept(serversocket, (struct sockaddr *)&host_addr, &addr_len);
    if (clientsocket == -1) {
        perror("accept");
        return;
    }
    setNonBlocking(clientsocket);
    struct epoll_event client_event;
    client_event.events = EPOLLIN | EPOLLOUT;
    client_event.data.fd = clientsocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientsocket, &client_event) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }
    std::map<int, std::pair<std::string, std::string> > ipPorts = data->getIpPorts();
    std::map<int, std::pair<std::string, std::string> >::iterator it;
    it = ipPorts.lower_bound(serversocket);
    if (it != ipPorts.end())
        data->fillIpPorts(clientsocket, it->second.first, it->second.second);
    Client *req = new Client();
    data->getClientMap()[clientsocket] =  req;
    std::cout << "Accepted connection on socket." << std::endl;
}

void parseRequestAndHeaders(Client* client) {
    std::size_t requestLineEnd = client->body.find("\r\n");
    if (requestLineEnd == std::string::npos) {
        // error handling
        return;
    }
    std::string requestLine = client->body.substr(0, requestLineEnd);

    std::istringstream requestLineStream(requestLine);
    std::string method, url, version;
    requestLineStream >> method >> url >> version;

    client->request["method"] = method;
    client->request["url"] = url;
    client->request["version"] = version;

    std::size_t headersStart = requestLineEnd + 2; // Move past "\r\n"
    std::size_t headersEnd = client->body.find("\r\n\r\n");

    std::string headers = client->body.substr(headersStart, headersEnd - headersStart);

    std::istringstream headersStream(headers);
    std::string header;
    while (std::getline(headersStream, header)) {
        std::size_t pos = header.find(":");
        if (pos != std::string::npos) {
            std::string key = header.substr(0, pos);
            std::string value = header.substr(pos + 1);
            client->request[key] = value;
        }
    }
    // Remove headers from body
    client->body = client->body.substr(headersEnd + 4);
    client->Request = true;
}

void handleIncomingData(int socket, Data* data) 
{
    char buffer[BUFFER_SIZE];
    
    if (read(socket, buffer, BUFFER_SIZE) == -1) {
        std::cerr << "read failed" << std::endl; 
        close(socket);
        return;
    }
    data->getClientMap()[socket]->body.append(buffer);
    std::size_t found = data->getClientMap()[socket]->body.find("\r\n\r\n");
    if (found != std::string::npos)
        parseRequestAndHeaders(data->getClientMap()[socket]);
    // std::cout << "Received request on socket." << std::endl;
    /******test***********/
    // std::map<int, std::pair<std::string, std::string> > map = data->getIpPorts();
    // std::map<int, std::pair<std::string, std::string> >::iterator it = map.lower_bound(socket);
    // std::cout << "ip: " << it->second.first << " port: " << it->second.second << std::endl;
}

void handleOutgoingData(int epoll_fd, int socket, Data *data) 
{
    // if (data->getClientMap()[socket]->Respond == false)
    //     return;
    std::map<int, Client*>::iterator it = data->getClientMap().lower_bound(socket);
    if (it !=  data->getClientMap().end())
    {
    std::cout << "respond flag: " << data->getClientMap()[socket]->Respond << std::endl;
        std::string resp1 = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>Waaaaaaaaaaaaaaaaaaaaaaa3</html>\r\n";
        
        write(socket, resp1.c_str(), resp1.size());
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, socket, NULL);
        close(socket);
        delete it->second;
        data->getClientMap().erase(it);
        // std::cout << "Closed connection on socket." << std::endl;
        // data->getIpPorts().erase(socket);
    } 
    // else
    //     close(socket);
}

void startTheServer(Data *data)
{
    std::map<int, Client*> clientMap;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
        throw("epoll_create1");

    int serversocket;
    std::vector<int> serversockets;
    servers_map servers = data->getServers();
    struct sockaddr_in host_addr[servers.size()];
    socklen_t addr_len[servers.size()];
    servers_map::iterator it = servers.begin();

    int i = 0;
    for (; it != servers.end(); it++) 
    {
        serversocket = createAndBindSocket(it, host_addr[i], addr_len[i]);
        if (serversocket != -1) 
        {
            addSocketToEpoll(epoll_fd, serversocket);
            std::cout << "Socket " << i + 1 << " created, bound, and listening." << std::endl;
            
        }
        serversockets.push_back(serversocket);
        data->fillIpPorts(serversocket, it->first.first, it->first.second);
        i++;
    }
    while (1) {
        struct epoll_event ev[MAX_EVENTS];
        int num_events = epoll_wait(epoll_fd, ev, MAX_EVENTS, -1);
        if (num_events == -1) 
            throw("epoll_wait");
        for (int i = 0; i < num_events; i++) {
            if (easyfind(serversockets, ev[i].data.fd) != serversockets.end())
                handleIncomingConnection(data, epoll_fd, ev[i].data.fd, host_addr[i], addr_len[i]);
            else if (ev[i].events & EPOLLIN) 
                handleIncomingData(ev[i].data.fd, data);
            else if (ev[i].events & EPOLLOUT) 
                handleOutgoingData(epoll_fd, ev[i].data.fd, data);
        }
    }
}