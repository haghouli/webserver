#include "../../Methodes/includes/get.hpp"
#include "../includes/Multi.hpp"

int shouldClose = 0;

void sigpipeHandler(int sig) {
    (void)sig;
    shouldClose = 1;
}

int createAndBindSocket(servers_map::iterator &it)
{
    int serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket == -1) 
        throw (std::runtime_error("Error: Fialed To Create A socket"));
    struct sockaddr_in host_addr;
    socklen_t addr_len = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(getPort(it->first.second));
    host_addr.sin_addr.s_addr = getIp(it->first.first);
    if (bind(serversocket, (struct sockaddr *)&host_addr, addr_len) != 0)
        throw (std::runtime_error("Error: Operation Failed Binding " + it->first.first + ":" + it->first.second));
    if (listen(serversocket, SOMAXCONN) != 0) 
        throw (std::runtime_error("Error: Listen Operation Failed"));
    return serversocket;
}

void handleIncomingConnection(Data *data, int epoll_fd, int serversocket) 
{
    struct sockaddr_in host_addr;
    socklen_t addr_len = sizeof(host_addr);
    int clientsocket = accept(serversocket, (struct sockaddr *)&host_addr, &addr_len);
    if (clientsocket == -1)
        throw (std::runtime_error("Error: Client Connection Refused"));
    struct epoll_event client_event;
    client_event.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR;
    client_event.data.fd = clientsocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientsocket, &client_event) == -1)
        throw (std::runtime_error("Error: Failed Adding Cient Socket To Epoll Instance"));
    std::map<int, std::pair<std::string, std::string> > ipPorts = data->getIpPorts();
    std::map<int, std::pair<std::string, std::string> >::iterator it;
    it = ipPorts.lower_bound(serversocket);
    if (it != ipPorts.end())
        data->fillIpPorts(clientsocket, it->second.first, it->second.second);
    Client *client = new Client();
    data->getClientMap()[clientsocket] =  client;
}


void handleIncomingData(int epoll_fd, int socket, Data* data) 
{
    Client * currentClient = data->getClientMap()[socket];
    ssize_t bytes;
    char buffer[BUFFER_SIZE + 1];
    bytes = read(socket, buffer, BUFFER_SIZE);
    buffer[bytes] = '\0';
    currentClient->bodySize         += bytes;
    currentClient->contentLength    += bytes;
    if (bytes <= 0) {
        dropClient(epoll_fd, socket, data);
        return;
    }
    currentClient->body.append(buffer, bytes);
    std::size_t found = currentClient->body.find("\r\n\r\n");
    if (found != std::string::npos && !currentClient->Request)
    {
        bytes -= (found + 4);
        currentClient->bodySize         -= (found + 4);
        currentClient->contentLength    -= (found + 4);
        currentClient->res.code = parseRequestAndHeaders(currentClient);
        fillErrorPages(currentClient, data->findServer(data->getIpPorts()[socket], currentClient->request["Host"]));
        if(currentClient->res.code == 200) {
            currentClient->check_request(data, socket);
        }
        if(currentClient->res.code != 200) {
            currentClient->Request = true;
            currentClient->Respond = true;
        }
    }
    if (currentClient->Request == true){
        if(currentClient->res.code == 200) {
            std::string Method = currentClient->request["method"];
            if(Method == "GET") {
                Get get(currentClient);
                get.getResponse();
            } 
            else if(Method == "POST") {
                gettimeofday(&currentClient->postStartTime, NULL);
                postRequest(*currentClient);
            } else if(Method == "DELETE"){
                deleteRequest(*currentClient);
            }
        }
        ResponseGenerator::getResponse(currentClient);
    }
}

void handleOutgoingData(int epoll_fd, int socket, Data *data)
{   
    Client *currentClient = data->getClientMap()[socket];
    if (!checkForTimeOut(currentClient, epoll_fd, socket, data))
        return;
    if (currentClient->Request
        && (!currentClient->isFirstTime 
        || currentClient->isCgiInProcess
        || checkForBoundary(currentClient)))
        {
            if(currentClient->res.code == 200) 
            {
                std::string Method = currentClient->request["method"];
                if(Method == "GET") {
                    Get get(currentClient);
                    get.getResponse();
                } 
                else if(Method == "POST") {
                    gettimeofday(&currentClient->postStartTime, NULL);
                    postRequest(*currentClient);
                }
            }
            ResponseGenerator::getResponse(currentClient);
        }
        if (!currentClient->Respond)
            return;
        currentClient->isFirstTime = false;
        std::string resp = currentClient->respond;
        int len = write(socket, resp.c_str(), resp.size());
        if (len <= 0 || !currentClient->isChunck){
            dropClient(epoll_fd, socket, data);
        }
}

void startTheServer(Data *data)
{
    int i = 0;
    int serversocket;
    std::map<int, Client*> clientMap;
    std::vector<int> serversockets;
    servers_map servers = data->getServers();
    size_t errorCounter = 0;
    servers_map::iterator it = servers.begin();
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Error: Failed Create Epoll Instance\n"; return;
    }
    for (; it != servers.end(); it++) {
        try{
            serversocket = createAndBindSocket(it);
            addSocketToEpoll(epoll_fd, serversocket);
            std::cout << "Socket " << i + 1 << " created, bound, and listening on " 
            << it->first.first << ":"<< it->first.second << std::endl;    
            serversockets.push_back(serversocket);
            data->fillIpPorts(serversocket, it->first.first, it->first.second);
            i++;
        }
        catch (std::exception &e){
            std::cerr << e.what() << std::endl; errorCounter++;
        }
    }
    if (errorCounter == servers.size())
        return;
    signal(SIGPIPE, SIG_IGN);
    while (1) {
        struct epoll_event ev[MAX_EVENTS];
        int num_events = epoll_wait(epoll_fd, ev, MAX_EVENTS, -1);
        if (num_events == -1){
            std::cerr << "Error: epoll_wait Failed" << std::endl; return;
        } 
        signal(SIGPIPE, sigpipeHandler);
        for (int i = 0; i < num_events; i++) {
            if (shouldClose) {
                dropClient(epoll_fd, ev[i].data.fd, data);
                shouldClose = 0;
                continue;
            }
            try{
                if (ev[i].events & (EPOLLHUP | EPOLLERR)) {
                    dropClient(epoll_fd, ev[i].data.fd, data);
                } 
                else if (easyfind(serversockets, ev[i].data.fd) != serversockets.end())
                    handleIncomingConnection(data, epoll_fd, ev[i].data.fd);
                else if ((ev[i].events & EPOLLIN) && !data->getClientMap()[ev[i].data.fd]->Respond) 
                    handleIncomingData(epoll_fd, ev[i].data.fd, data);
                else
                    handleOutgoingData(epoll_fd, ev[i].data.fd, data);
            }
            catch (std::exception &e){
                std::cerr << e.what() << std::endl;
            }
        }
    }
}