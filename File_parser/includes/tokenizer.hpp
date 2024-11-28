#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "./response.hpp"

enum types {DIRECTIVE, CONTEXT_SERVER, CONTEXT_LOCATION, CONTEXT_TYPES, CONTEXT_CGI, LBRACE, RBRACE, SEMICOLON, NUMBER, STRING, END};

typedef struct s_token {
    types       type;
    std::string value;
    size_t      index;
}           t_token;


class tokenizer {
    std::vector<std::string>    contexts;
    std::vector<std::string>    directives;
    std::vector<t_token>        tokens;

    void                        save_token(std::string value, types type, int idx);
    void                        skip_spaces(std::string str, size_t & idx);
    void                        get_others(std::string str,size_t & idx);

    public:
        tokenizer();
        std::vector<t_token>    generate_tokens(std::string text);
        void                    print_tokens(std::vector<t_token> tokens);

};

#endif