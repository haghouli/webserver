#ifndef GET_HPP
#define GET_HPP

#include "../../cgi/cgi.hpp"


class Get {
    private:
        std::string                         request_content;
        Client                              * client;

    public:
        Get(Client * client);

        void                                getResponse();
        std::string                         getDires();
        void                                getFileResponse(std::string extention);
        void                                autoIndexOff();
        bool                                isFileIndexExist(std::string & extention, std::string & file_path);

        void                                cgi(Client * client, std::string extention);

        void                                getRequestedDir();
        void                                getRequestedFile();
        std::string                         getExistedIndex();
};


#endif