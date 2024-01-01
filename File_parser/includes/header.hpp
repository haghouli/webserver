#include <iostream>
#include <map>
#include <vector>

typedef std::map<std::string, std::vector<std::string> > vector_map;

class Methodes
{
    public:
        std::string Get();
        std::string Post();
        std::string Delete();

};

class location {
    public:
        vector_map                                          directives;
        Methodes                                            methodes;
        std::map<std::string, std::string>                  cgis;

};

class server {
    public:
        vector_map                                          directives;
        std::map<std::string, location*>                    locations;
        std::map<std::string, std::string>                  mime_types;

        std::map<int , std::string>                         error_pages; 
};

class Client {
    public:
        std::map<std::string, std::string> request;
        std::string respond;
        Client(std::string &str);
        ~Client();
};