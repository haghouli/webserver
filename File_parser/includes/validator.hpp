#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include "./errors.hpp"
#include "./response.hpp"

class validator {

    private:
        std::vector<server*>                                servers;
        std::map<std::string, std::vector<std::string> >    defaults;

        bool    isValidPort(std::string port);
        bool    valid_allow_methodes(std::vector<std::string> v);
        bool    valid_return(std::vector<std::string> v);
        bool    valid_autoindex(std::vector<std::string> v);
        bool    valid_error_page(std::vector<std::string> v, server * server);
        bool    valid_prefix_value(std::vector<std::string> v, int n);
        bool    is_valid_listen(std::vector<std::string> v, server * server);
        bool    is_valid_ip(std::string s);
        void    validate_directive(vector_map::iterator it2, server * server);
        bool    checkServerNames(std::vector<server*> v1, std::vector<std::string> v2);
        void    setDefaultDirectives(server * s);

        void    DeleteAndThrow(std::string paramName);

    public:
        validator(std::vector<server*> & servers);
        std::vector<server*>            getServers();
        void                            eliminateDuplicateServers(servers_map & all_servers);

        void validation();

};

#endif