#include "../includes/collector.hpp"
#include "../includes/validator.hpp"

Data * getConfig(int ac, char **av)
{
        if (ac < 2)
            throw "Need atleast on param";
        if (ac > 2)
            throw "Too many params";
        std::string text = check_file_errors(av[1]);
        tokenizer t;
        std::vector<t_token> tokens = t.generate_tokens(text);

        if (tokens.empty())
            throw false;

        collector c(tokens);
        c.check_synthax();

        std::vector<server*> servers;
        c.collecte_data(servers);

        validator v(servers);
        v.validation();

        servers_map all_servers;
        v.eliminateDuplicateServers(all_servers);
    
        Data * data = new Data(all_servers);

        return data;
}