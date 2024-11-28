#include "../includes/collector.hpp"

collector::collector(std::vector<t_token> tkns) {
    std::string s = "listen server_name error_page client_max_body_size";

    duplicat = splite(s, " ");
    tokens = tkns;
}

bool collector::check_ahead(size_t idx, size_t to_check)
{
    for (size_t i = idx; i < to_check; i++)
        if (tokens[i].type == END)
            return false;
    return true;
}

void collector::key_value_synthax(size_t &idx)
{
    if (!check_ahead(idx, 3))
        throw synthax_error(tokens[idx].value);
    if (tokens[idx].type != STRING
        || tokens[++idx].type != STRING
        || tokens[++idx].type != SEMICOLON)
        throw synthax_error(tokens[idx].value);;
    idx++;
}

void collector::cgi_and_types_synthax(size_t & idx, types type)
{
    if (!check_ahead(idx, 6))
        throw synthax_error(tokens[idx].value);
    if (tokens[idx].type != type
        || tokens[++idx].type != LBRACE
        || tokens[++idx].type != STRING)
        throw synthax_error(tokens[idx].value);;
    while(tokens[idx].type != RBRACE && tokens[idx].type != END)
        key_value_synthax(idx);
    if(tokens[idx].type != RBRACE)
        throw synthax_error(tokens[idx].value);
    idx++;
}

void collector::direcrive_synthax(size_t & idx)
{
    if (!check_ahead(idx, 3))
        throw synthax_error(tokens[idx].value);;
    if (tokens[idx].type != DIRECTIVE || tokens[++idx].type != STRING)
        throw synthax_error(tokens[idx].value);
    while (tokens[idx].type == STRING)
        idx++;
    if (tokens[idx].type != SEMICOLON)
        throw synthax_error(tokens[idx].value);;
    idx++;
}

void collector::location_synthax(size_t & idx)
{
    if (!check_ahead(idx, 7))
        throw synthax_error(tokens[idx].value);
    if (tokens[idx].type != CONTEXT_LOCATION
        || tokens[++idx].type != STRING
        || tokens[++idx].type != LBRACE)
        throw synthax_error(tokens[idx].value);
    idx++;
    if(tokens[idx].type != DIRECTIVE 
        && tokens[idx].type != CONTEXT_CGI
        && tokens[idx].type != RBRACE)
        throw synthax_error(tokens[idx].value);
    while (tokens[idx].type == DIRECTIVE || tokens[idx].type == CONTEXT_CGI)
    {
        if(tokens[idx].type == DIRECTIVE)
            direcrive_synthax(idx);
        else if(tokens[idx].type == CONTEXT_CGI)
            cgi_and_types_synthax(idx, CONTEXT_CGI);    
    }
    if(tokens[idx].type != RBRACE)
        throw synthax_error(tokens[idx].value);
    idx++;
}

void collector::server_synthax(size_t & idx) {
    if(!check_ahead(idx, 3))
        throw synthax_error(tokens[idx].value);
    if(tokens[idx].type != CONTEXT_SERVER || tokens[++idx].type != LBRACE)
        throw synthax_error(tokens[idx].value);
    idx++;
    if(tokens[idx].type != CONTEXT_LOCATION
        && tokens[idx].type != DIRECTIVE
        && tokens[idx].type != RBRACE
        && tokens[idx].type != CONTEXT_TYPES
        && tokens[idx].type != CONTEXT_CGI)
        throw synthax_error(tokens[idx].value);
    while (tokens[idx].type == DIRECTIVE 
        || tokens[idx].type == CONTEXT_CGI
        || tokens[idx].type == CONTEXT_LOCATION
        || tokens[idx].type == CONTEXT_TYPES)
    {
        if(tokens[idx].type == DIRECTIVE)
            direcrive_synthax(idx);
        else if(tokens[idx].type == CONTEXT_CGI)
            cgi_and_types_synthax(idx, CONTEXT_CGI);
        else if(tokens[idx].type == CONTEXT_TYPES)
            cgi_and_types_synthax(idx, CONTEXT_TYPES);
        else if(tokens[idx].type == CONTEXT_LOCATION)
            location_synthax(idx);
    }
    if(tokens[idx].type != RBRACE)
        throw synthax_error(tokens[idx].value);
    idx++;
}

void collector::check_synthax() {
    size_t i = 0;
    while (tokens[i].type != END)
        server_synthax(i);
}

void    collector::collecte_directives(size_t & idx, vector_map & directives, int b, std::vector<server*> & serveres) {
    std::string d = tokens[idx++].value;
    vector_map::iterator it = directives.find(d);
    if(b == 1)
        if(find(duplicat.begin(), duplicat.end(), d) != duplicat.end()) {
            deleteSeversVector(serveres);
            throw not_allowed_error(d);
        }
    if(b == 0 && d == "alias") {
        deleteSeversVector(serveres);
        throw not_allowed_error(d);
    }

    if(it != directives.end() && d != "server_name" && d != "error_page") {
        deleteSeversVector(serveres);
        throw duplicate_error(d);
    }

    int count = 0;

    if((d == "root" && directives.find("alias") != directives.end())
        || (d == "alias" && directives.find("root") != directives.end())) {
        deleteSeversVector(serveres);
        throw "root and alias can't both be at the same location";
    }

    while (tokens[idx].type != SEMICOLON) {
        directives[d].push_back(tokens[idx++].value);
        count++;
    }
    if((d == "listen" || d == "server_name") && count > 1) {
        deleteSeversVector(serveres);
        throw "Error in params number";
    }
    idx++;
}

std::map<std::string, std::string> collector::collecte_key_values(size_t & idx) {
    std::map<std::string, std::string> mime_types;
    idx+=2;
    while (tokens[idx].type != RBRACE)
    {
        mime_types[tokens[idx].value] = tokens[idx + 1].value;
        idx+=3;
    }
    idx++;
    return mime_types;     
}

void collector::collecte_locations(size_t & idx, server * s, std::vector<server*> & serveres) {
    location *l = new location();
    idx++;
    std::map<std::string, location*>::iterator it = s->locations.find(tokens[idx].value);
    if(it != s->locations.end()) {
        deleteSeversVector(serveres);
        delete l;
        throw duplicate_error("location " + tokens[idx].value);
    }
    s->locations[tokens[idx++].value] = l;
    idx++;
    while(tokens[idx].type != RBRACE) {
        if(tokens[idx].type == DIRECTIVE)
            collecte_directives(idx, l->directives, 1, serveres);
        else if (tokens[idx].type == CONTEXT_CGI)
            l->cgis = collecte_key_values(idx);
    }
    idx++;
}

void collector::collecte_data(std::vector<server*> & serveres) {
    for(size_t i = 0; i < tokens.size(); i++) {
        if(tokens[i].type == CONTEXT_SERVER) {
            server *s = new server();
            serveres.push_back(s);
            vector_map directives;
            i+=2;
            while (tokens[i].type != RBRACE)
            {
                if(tokens[i].type == DIRECTIVE)
                    collecte_directives(i, directives, 0, serveres);
                if(tokens[i].type == CONTEXT_CGI)
                    s->cgis = collecte_key_values(i);
                else if(tokens[i].type == CONTEXT_TYPES) {
                    if(s->mime_types.empty())
                        s->mime_types = collecte_key_values(i);
                    else {
                        deleteSeversVector(serveres);
                        throw duplicate_error(tokens[i].value + " context");
                    }
                }
                else if(tokens[i].type == CONTEXT_LOCATION)
                    collecte_locations(i, s, serveres);
            }
            s->directives = directives;
        }
    }
}