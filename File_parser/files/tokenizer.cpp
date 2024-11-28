#include "../includes/tokenizer.hpp"

tokenizer::tokenizer() {
   std::string dirvs = "listen server_name index root allow_methods client_max_body_size return error_page \
   autoindex uploads alias include request_cgi_timeout";

   directives = splite(dirvs, " ");
}

void   tokenizer::skip_spaces(std::string str, size_t & idx) {
    while (str[idx] == '\n' || str[idx] == ' ' || str[idx] == '\t')
        idx++;
}

void    skip_comments(std::string text, size_t & idx) {
    if(text[idx] == '#') {
        while (text[idx] != '\n') {
            idx++;
        }
    }
}

void    tokenizer::get_others(std::string str,size_t & idx) {
    std::string word;
    size_t start_idx = idx;
    t_token token;

    if(str[idx] == '\"') {
        word += '\"';
        idx++;
        while(str[idx] != '\"' && str[idx] != '\0') {
            word += str[idx++];
        }
        if(str[idx] == '\0')
            throw "Synthax error";
        if(str[idx++] == '\"')
            word += "\"";
    } else {
        while(str[idx] != '\n' && str[idx] != ' ' && str[idx] != '\t' &&
        str[idx] != ';' && str[idx] != '{' && str[idx] != '}' && str[idx]
        && str[idx] != '\"' )
            word += str[idx++];   
    }
    token.index = start_idx;
    token.value = word;

    if(word.empty())
        return;
    else if(word == "server")
        token.type = CONTEXT_SERVER;
    else if(word == "location")
        token.type = CONTEXT_LOCATION;
    else if(word == "cgis")
        token.type = CONTEXT_CGI;
    else if(word == "types")
        token.type = CONTEXT_TYPES;
    else if(find(directives.begin(), directives.end(), word) != directives.end())
        token.type = DIRECTIVE;
    else {
        token.value = strTrim(token.value, "\"");
        if(token.value.empty())
            throw "Empty argument";
        token.type = STRING;
    }
    tokens.push_back(token);
}

void tokenizer::save_token(std::string value, types type, int idx) {
    t_token token;

    token.type = type;
    token.value = value;
    token.index = idx;

    tokens.push_back(token);
}

std::vector<t_token> tokenizer::generate_tokens(std::string text) {
    size_t i = 0;
    while(text[i]) {
        skip_spaces(text, i);
        skip_comments(text, i);
        switch (text[i])    
        {
            case '{': save_token("{", LBRACE, i++); break;
            case '}': save_token("}", RBRACE, i++); break;
            case ';': save_token(";", SEMICOLON, i++); break;
            case '\0': save_token("\0", END, i); break;
            default:
                get_others(text, i);
                break;
        }
    }
    return tokens;
}