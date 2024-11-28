#ifndef COLLECTOR_HPP
#define COLLECTOR_HPP

#include "./errors.hpp"
#include "./tokenizer.hpp"

class collector {

    private:
       std::vector<t_token> tokens;
       std::vector<std::string> duplicat;

    public:
        collector(std::vector<t_token> tkns);

        void                                check_synthax();
        void                                collecte_directives(size_t & idx, vector_map & directives, int b, std::vector<server*> & serveres);
        std::map<std::string, std::string>  collecte_key_values(size_t & idx);
        void                                collecte_locations(size_t & idx, server * s, std::vector<server*> & serveres);
        void                                collecte_data(std::vector<server*> & servers);
        bool                                check_ahead(size_t idx, size_t to_check);
        void                                key_value_synthax(size_t &idx);
        void                                cgi_and_types_synthax(size_t & idx, types type);
        void                                direcrive_synthax(size_t & idx);
        void                                location_synthax(size_t & idx);
        void                                server_synthax(size_t & idx);
};


#endif