#include "../includes/validator.hpp"


void checkMimeTypes(server * s) {
    if(s->directives.find("include") != s->directives.end()) {
        std::map<std::string, std::string> allMimeTypes = getAllMimeTypes();
        std::map<std::string, std::string>::iterator it = allMimeTypes.begin();
        for(; it != allMimeTypes.end(); it++) {
            if(s->mime_types.find(it->first) == s->mime_types.end())
                s->mime_types[it->first] = it->second;
        }
    }
}

validator::validator(std::vector<server*> & servers) : servers(servers) {
    defaults["client_max_body_size"].push_back("1024M");
    defaults["autoindex"].push_back("off");
    defaults["index"].push_back("index.html");
    defaults["index"].push_back("index.htm");
    defaults["uploads"].push_back("on");
    defaults["allow_methods"].push_back("GET");
    defaults["allow_methods"].push_back("POST");
    defaults["allow_methods"].push_back("DELETE");
    defaults["request_cgi_timeout"].push_back("5s");
}

std::vector<server*> validator::getServers() {
    return servers;
}

bool validator::valid_allow_methodes(std::vector<std::string> v) {
    std::vector<std::string> strs = splite("GET POST DELETE", " ");
    std::vector<std::string>::iterator it = v.begin();

    if(has_duplicate(v))
        return false;

    for(; it != v.end(); it++)
        if(find(strs.begin(), strs.end(), *it) == strs.end())
            return false;
    
    return true;
}

bool validator::valid_return(std::vector<std::string> v) {
    if(v.size() != 2 || v[0].empty() || v[1].empty())
        return false;
    if(!is_number(v[0]) || is_number(v[1]))
        return false;
    if(atoi(v[0].c_str()) < 300 || atoi(v[0].c_str()) >= 400)
        return false;
    return true;
}

bool validator::valid_autoindex(std::vector<std::string> v) {
    std::vector<std::string> strs = splite("on off", " ");
    if(v.size() != 1 || v[0].empty())
        return false;
    if(find(strs.begin(), strs.end(), v[0]) == strs.end())
        return false;
    return true;
}

bool validator::valid_error_page(std::vector<std::string> v, server * server) {
    if(v.size() % 2 != 0)
        return false;
    for(size_t i = 0; i < v.size(); i++) {
        if(i % 2 == 0 && !is_number(v[i]))
            return false;
        std::string key = v[i++];
        if(key.empty())
            return false;
        if(atoi(key.c_str()) < 400 || atoi(key.c_str()) >= 600)
            return false;
        if(i % 2 != 0 && is_number(v[i]))
            return false;
        if(server->error_pages.find(atoi(key.c_str())) != server->error_pages.end()) {
            deleteSeversVector(this->servers);
            throw duplicate_error("error_page");
        }
        if(v[i].empty())
            return false;
        server->error_pages[atoi(key.c_str())] = getRealPath(v[i].c_str());
    }
    return true;
}

bool validator::valid_prefix_value(std::vector<std::string> v, int n) {
    if(v.size() != 1 || v[0].empty())
        return false;

    std::string number = v[0].substr(0, v[0].size() - 1);
    std::string prefix = v[0].substr(v[0].size() - 1, 1);
    std::string prfx;

    n == 0 ? (prfx = "s" ) : prfx = "M";

    if(!is_number(number) || prfx.find(prefix) == std::string::npos)
        return false;
    char * str = NULL;
    int num = std::strtol(number.c_str(), &str, 10);
    if(errno == ERANGE || num < 0)
        return false;
    return true;
}

bool validator::is_valid_ip(std::string s) {
    std::vector<std::string> v = splite(s, ".");
    if(v.size() != 4)
        return false;
    for(size_t i = 0; i < v.size(); i++) {
        if(v[i].size() > 3 || !is_number(v[i]))
            return false;
    }
    return true;
}

std::string fix_ip(std::string ip) {
    std::vector<std::string> v = splite(ip, ".");
    std::vector<std::string> v2;

    std::vector<std::string>::iterator it = v.begin();
    for(; it != v.end(); it++) {
        std::string s;
        int i = 0;
        while((*it)[i] == '0')
            i++;
        while ((*it)[i])
            s += (*it)[i++];
        if(s.empty())
            s = "0";
        v2.push_back(s);
    }

    it = v2.begin();
    std::string fixed_ip;
    for(; it != v2.end(); it++) {
        fixed_ip += *it;
        if(it + 1 != v2.end())
            fixed_ip += ".";
    }
    return fixed_ip;
}

bool validator::isValidPort(std::string port) {
    size_t n = strtol(port.c_str(), NULL, 10);
    if(errno == ERANGE || n >  MAX_PORT)
        return false;
    return true;
}

bool validator::is_valid_listen(std::vector<std::string> v, server * server) {
    std::vector<std::string> listen_elements;
    std::string ip, port;

    if(v.size() != 1 || v[0].empty())
        return false;
    if(v[0][0] == ':' || v[0][v[0].size() - 1] == ':')
        return false;
    std::vector<std::string> v2 = splite(v[0], ":");
    if(v2.size() > 2)
        return false;
    if(v2.size() == 1) {
        if(is_valid_ip(v2[0]))
            ip = fix_ip(v2[0]);
        else if(is_number(v2[0]) && isValidPort(v2[0]))
            port = fix_ip(v2[0]);
        else
            return false;
    }
    else if(v2.size() == 2) {
        if(!is_valid_ip(v2[0]) || !is_number(v2[1]) || !isValidPort(v2[1]))
            return false;
       ip = fix_ip(v2[0]);
       port = v2[1]; 
    }
    ip.empty() ? listen_elements.push_back("0.0.0.0") : listen_elements.push_back(ip);
    port.empty() ? listen_elements.push_back("80") : listen_elements.push_back(port);
    server->directives["listen"].clear();
    server->directives["listen"] = listen_elements;
    return true;

}

bool valid_uploads(std::vector<std::string> v) {
    if(v.size() < 1 || v.size() > 2)
        return false;
    if(v[0] != "on" && v[0] != "off")
        return false;
    if(v[0] == "off") {
        if(v.size() > 1)
            return false;
    }
    else {
        if(v.size() != 2 || is_number(v[1]))
            return false;
    }
    return true;
}

bool isValidInclude(std::vector<std::string> v) {
    if(v.size() != 1 || v[0] != "mime.types")
        return false;
    return true;
}

void validator::DeleteAndThrow(std::string paramName) {
    deleteSeversVector(this->servers);
    throw params_error(paramName);
}

void validator::validate_directive(vector_map::iterator it2, server * server) {
    if(it2->first == "listen" && !is_valid_listen(it2->second, server))
        DeleteAndThrow(it2->first);
    else if(it2->first == "server_name" && (has_duplicate(it2->second) || !is_all_string(it2->second)))
        DeleteAndThrow(it2->first);
    else if((it2->first == "root" || it2->first == "alias") 
        && (it2->second.size() > 1 || !is_all_string(it2->second)))
        DeleteAndThrow(it2->first);
    else if(it2->first == "index" && !is_all_string(it2->second))
        DeleteAndThrow(it2->first);
    else if(it2->first == "allow_methods" && !valid_allow_methodes(it2->second))
        DeleteAndThrow(it2->first);
    else if(it2->first == "return" && !valid_return(it2->second))
        DeleteAndThrow(it2->first);
    else if(it2->first == "autoindex" && !valid_autoindex(it2->second))
        DeleteAndThrow(it2->first);
    else if(it2->first == "error_page" && !valid_error_page(it2->second, server))
        DeleteAndThrow(it2->first);
    else if(it2->first == "request_cgi_timeout" && !valid_prefix_value(it2->second, 0))
        DeleteAndThrow(it2->first);
    else if(it2->first == "client_max_body_size" && !valid_prefix_value(it2->second, 1))
        DeleteAndThrow(it2->first);
    else if(it2->first == "uploads" && !valid_uploads(it2->second))
        DeleteAndThrow(it2->first);
    else if(it2->first == "include" && !isValidInclude(it2->second))
        DeleteAndThrow(it2->first);
    if((it2->first == "root" || it2->first == "alias") && it2->second[0][it2->second[0].size() - 1] == '/')
        it2->second[0] = it2->second[0].substr(0, it2->second[0].size() - 1);
}

void validator::validation() {
    
    std::vector<server*>::iterator it = servers.begin();
    
    for(; it != servers.end(); it++) {
        vector_map::iterator it2 = (*it)->directives.begin();
        for(; it2 != (*it)->directives.end(); it2++)
            validate_directive(it2, *it);

        std::map<std::string, location*>::iterator it3 = (*it)->locations.begin();
    
        for(; it3 != (*it)->locations.end(); it3++) {
            vector_map::iterator it4 = it3->second->directives.begin();
            for(; it4 != it3->second->directives.end(); it4++)
                validate_directive(it4, *it);
        }
        if((*it)->directives.find("listen") == (*it)->directives.end()) {
            (*it)->directives["listen"].push_back("0.0.0.0");
            (*it)->directives["listen"].push_back("80");
        }
        if((*it)->directives.find("server_name") == (*it)->directives.end())
            (*it)->directives["server_name"].push_back((*it)->directives["listen"][0]);
    }
}

bool validator::checkServerNames(std::vector<server*> v1, std::vector<std::string> v2) {
  
    std::vector<std::string>::iterator it2 = v2.begin();

    for(; it2 != v2.end(); it2++) {
        std::vector<server*>::iterator it1 = v1.begin();
        int counter = 0;
        for(; it1 != v1.end(); it1++) {
            if(find((*it1)->directives["server_name"].begin(), (*it1)->directives["server_name"].end(), *it2) != (*it1)->directives["server_name"].end())
                counter++;
        }
        if(counter == 0)
            return false;
    }
    return true;
}

void    validator::setDefaultDirectives(server * s) {
    std::map<std::string, std::vector<std::string> >::iterator it = defaults.begin();
    for(; it != defaults.end(); it++) {
        if(s->directives.find(it->first) == s->directives.end()) {
            std::vector<std::string>::iterator it2 = it->second.begin();
            for(; it2 != it->second.end(); it2++)
                s->directives[it->first].push_back(*it2);
        }
    }
    if(s->directives["uploads"][0] == "on" && s->directives["uploads"].size() == 1)
        s->directives["uploads"].push_back(s->directives["root"][0]);
}

void    validator::eliminateDuplicateServers(servers_map & all_servers) {
    
    std::vector<server*>::iterator it = servers.begin();

    for(; it != servers.end(); it++) {
        std::pair<std::string, std::string> p;
        p.first = (*it)->directives["listen"][0];
        p.second = (*it)->directives["listen"][1];
        if(all_servers.find(p) == all_servers.end() || !checkServerNames(all_servers[p], (*it)->directives["server_name"])) {
            if((*it)->directives.find("root") == (*it)->directives.end()) {
                deleteSeversVector(this->servers);
                throw "Root directive is mandatory";
            }
            all_servers[p].push_back(*it);
            setDefaultDirectives(*it);
            checkMimeTypes(*it);
        } else {
            deleteSeversVector(this->servers);
            throw "Duplicated server : " + p.first + ":" + p.second;
        }
    }
}