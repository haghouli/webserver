#include "../includes/errors.hpp"

std::string synthax_error(std::string s) {
    return "Error Nier \'" + s +"\'";
}

std::string not_allowed_error(std::string s) {
    return "Directive " + s + " Not allow inside location Context";
}

std::string duplicate_error(std::string s) {
    return "Duplicated " + s ;
}

std::string     params_error(std::string s) {
    return "params error in " + s + " directive";
}