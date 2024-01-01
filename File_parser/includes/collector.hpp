#ifndef COLLECTOR_HPP
#define COLLECTOR_HPP

#include "../includes/errors.hpp"

class collector {

    private:
       std::vector<t_token> tokens;
       std::vector<std::string> duplicat;

    public:
        collector(std::vector<t_token> tkns);

        void                                check_synthax();
        void                                collecte_directives(size_t & idx, vector_map & directives, int b);
        std::map<std::string, std::string>  collecte_key_values(size_t & idx);
        void                                collecte_locations(size_t & idx, server * s);
        std::vector<server*>                collecte_data();
        bool                                check_ahead(size_t idx, size_t to_check);
        void                                key_value_synthax(size_t &idx);
        void                                cgi_and_types_synthax(size_t & idx, types type);
        void                                direcrive_synthax(size_t & idx);
        void                                location_synthax(size_t & idx);
        void                                server_synthax(size_t & idx);
};


#endif