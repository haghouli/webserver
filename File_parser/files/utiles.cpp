#include "../includes/utiles.hpp"

std::vector<std::string> splite(std::string str, std::string delim) {
    std::vector<std::string> splited_elements;
    
    for(int i = 0; i < str.size(); i++) {
        std::string s;
        while (str[i] && delim.find(str[i]) != std::string::npos)
            i++;
        while (str[i] && delim.find(str[i]) == std::string::npos)
            s += str[i++];
        if(!s.empty())
            splited_elements.push_back(s);
    }
    return splited_elements;
}

std::string readfile(std::ifstream & file) {
    std::string content;
    std::string line;

    while (getline(file, line))
        content += line + '\n'; 
    file.close();
    return content;
}

bool    check_extention(std::string file_name) {
    std::string extention = file_name.substr(file_name.size() - 5);
    return extention != ".conf" ? false : true;
}

std::string check_file_errors(std::string file_name) {
    std::ifstream file;

    if(!check_extention(file_name))
        throw "Invalid extention";
    file.open(file_name.c_str());
    if(!file)
        throw strerror(errno);
    return readfile(file);
}


bool is_number(std::string str) {
    for(int  i = 0; i < str.size(); i++)
        if(!isdigit(str[i]))
            return false;
    return true;
}

bool                        has_duplicate(std::vector<std::string> v) {
    std::map<std::string, bool> m;
    std::vector<std::string>::iterator it = v.begin();

    for(; it != v.end(); it++)
        m[*it] = false;

    it = v.begin();
    for(; it != v.end(); it++) {
        if(m[*it] == true)
            return true;
        m[*it] = true;
    }

    return false;
}

bool is_all_number(std::vector<std::string> v) {
    std::vector<std::string>::iterator it = v.begin();
    for(; it != v.end(); it++)
        if(!is_number(*it))
            return false;
    
    return true;
}

bool is_all_string(std::vector<std::string> v) {
    std::vector<std::string>::iterator it = v.begin();
    for(; it != v.end(); it++)
        if(is_number(*it))
            return false;
    
    return true;
}


Data::Data(servers_map servers) : servers(servers) { }

servers_map Data::getServers() {
    return servers;
}

std::map<int, Client*> &Data::getClientMap()
{
    return clientMap;
}

server * Data::findServer(std::pair<std::string, std::string> host_port, std::string server_name) {
    
    if(servers.find(host_port) == servers.end())
        throw "Server Not Found At The config File";

    std::vector<server*>::iterator it2 = servers[host_port].begin();
    for(; it2 != servers[host_port].end(); it2++)
        if(find((*it2)->directives["server_name"].begin(),  (*it2)->directives["server_name"].end(), server_name) != (*it2)->directives["server_name"].end())
            return *it2;

    return servers[host_port][0];
}
