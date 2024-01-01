#include "../includes/collector.hpp"
#include "../includes/validator.hpp"

void printServer(server * s) {
    std::cout << "\t\tDirectives ---->" << std::endl;
    vector_map::iterator it = s->directives.begin();
    for(; it != s->directives.end(); it++)
    {
        std::cout << "\t\t\t" << it->first << " : ";
        std::vector<std::string> v = it->second;
        for(int i = 0; i < v.size(); i++)
            std::cout << v[i] << " "; 
        std::cout << std::endl;
    }

    std::cout << "\t\tMime types ---->" << std::endl;
    std::map<std::string, std::string>::iterator it2 = s->mime_types.begin();
    for(; it2 != s->mime_types.end(); it2++)
        std::cout << "\t\t\t" << it2->first << " : " << it2->second << std::endl;
    
    std::cout << "\t\tError pages ---->" << std::endl;
    std::map<int, std::string>::iterator it6 = s->error_pages.begin();
    for(; it6 != s->error_pages.end(); it6++)
        std::cout << "\t\t\t" << it6->first << " " << it6->second << std::endl;

    std::cout << "\t\tLocations ---->" << std::endl;
    std::map<std::string, location*>::iterator it3 = s->locations.begin();
    for(; it3 != s->locations.end(); it3++){
        std::cout << "\t\t\t" << it3->first << std::endl;
        vector_map::iterator it4 = it3->second->directives.begin();
        for(; it4 !=  it3->second->directives.end(); it4++)
        {
            std::cout << "\t\t\t\t" << it4->first << " : ";
            std::vector<std::string> v2 = it4->second;
            for(int i = 0; i < v2.size(); i++)
                std::cout << v2[i] << " "; 
            std::cout << std::endl;
        }
        std::cout << "\t\t\t\tCgis ---->" << std::endl;
        std::map<std::string, std::string>::iterator it5 = it3->second->cgis.begin();
        for(; it5 != it3->second->cgis.end() ;it5++)
            std::cout << "\t\t\t\t\t" << it5->first << " : " << it5->second << std::endl;
    }
}

void print_data(std::vector<server*> servers) {
    for(int j = 0; j < servers.size(); j++) {
        std::cout << "\t---------------------> server <--------------------" << std::endl;
        printServer(servers[j]);
        
    }
}

void    print_final_data(servers_map    all_servers) {
    
    servers_map::iterator itt = all_servers.begin();
    
    for(; itt != all_servers.end(); itt++) {
        std::cout << "---------------------> " << itt->first.first << ":" << itt->first.second << " <------------------" << std::endl;
        print_data(itt->second);
    }
}



bool check(std::vector<server*> v1, std::vector<std::string> v2) {
  
    std::vector<std::string>::iterator it2 = v2.begin();

    for(; it2 != v2.end(); it2++) {
        std::vector<server*>::iterator it1 = v1.begin();
        int counter = 0;
        for(; it1 != v1.end(); it1++) {
            if(find((*it1)->directives["server_name"].begin(), (*it1)->directives["server_name"].end(), *it2) != (*it1)->directives["server_name"].end()) {
                counter++;
            }
        }
        if(counter == 0) {
            return false;
        }
    }
    return true;
}

servers_map test(std::vector<server*> servers) {
    
    std::vector<server*>::iterator it = servers.begin();

    servers_map all_servers;

    for(; it != servers.end(); it++) {
        std::pair<std::string, std::string> p;
        p.first = (*it)->directives["listen"][0];
        p.second = (*it)->directives["listen"][1];
        if(all_servers.find(p) == all_servers.end())
            all_servers[p].push_back(*it);
        else
            if(!check(all_servers[p], (*it)->directives["server_name"]))
               all_servers[p].push_back(*it);
            else
                std::cerr << "server " << p.first << ":" << p.second << " will be ignored" << std::endl;
    }

    return all_servers;
}

Data * getConfig(int ac, char **av)
{
        if (ac < 2)
            throw "Need atleast on argument";
        if (ac > 2)
            throw "Too many arguments";
        std::string text = check_file_errors(av[1]);
        tokenizer t;
        std::vector<t_token> tokens = t.generate_tokens(text);
        // t.print_tokens(tokens);
        if (tokens.empty())
            throw false;
        collector c(tokens);
        c.check_synthax();

        std::vector<server*> servers = c.collecte_data();

        validator v(servers);
        v.validation();

        // print_data(servers);

        std::vector<server *> servers2 = v.getServers();

        servers_map all_servers = test(servers2);
    
        Data * data = new Data(all_servers);
        // print_final_data(data->getServers());

        return data;
}