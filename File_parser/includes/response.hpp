#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../includes/utiles.hpp"

class ResponseGenerator {
    private:
        ResponseGenerator();
        ~ResponseGenerator();
    
    public:
        static void getResponse(Client * client);
};


#endif