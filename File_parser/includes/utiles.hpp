#ifndef UTILES_HPP
#define UTILES_HPP

#include <iostream>
#include <fstream>
#include <cerrno>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <cstring>
#include <algorithm>
#include <ctype.h>
#include <stack>
#include <utility>


#include "../includes/tokenizer.hpp"
#include "../includes/errors.hpp"

//******* Multiplexing ***************
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sstream>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

class server;
class Client;

typedef std::map<std::pair<std::string, std::string>, std::vector<server*> > servers_map;

std::vector<std::string>  splite(std::string str, std::string delim);
std::string                                                                 readfile(std::ifstream & file);
std::string                                                                 check_file_errors(std::string file_name);
bool                                                                        is_number(std::string str);
bool                                                                        has_duplicate(std::vector<std::string> v);
bool                                                                        is_all_number(std::vector<std::string> v);
bool                                                                        is_all_string(std::vector<std::string> v);

class Data {
    private:
        servers_map servers;
        std::map<int, Client*>  clientMap;
        std::map<int, std::pair<std::string, std::string> > ipPorts;
    
    public:
        Data(servers_map servers);
        servers_map getServers();
        std::map<int, Client*> &getClientMap();
        server * findServer(std::pair<std::string, std::string> host_port, std::string server_name);
        void     fillIpPorts(int key, const std::string& value1, const std::string& value2);
        std::map<int, std::pair<std::string, std::string> > &getIpPorts(); 
};

Data *         getConfig(int ac, char **av);

#endif