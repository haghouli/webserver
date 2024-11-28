#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <cstring>
#include <algorithm>
#include <ctype.h>
#include <stack>
#include <utility>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sstream>
#include <ctime>
#include <cmath>
#include <stdexcept>
#include <sys/time.h>

#define MAX_PORT 65535

class Data;

typedef std::map<std::string, std::vector<std::string> > vector_map;

class response {
    public:
        int         code;
        int         contentSize;
        std::string content;
        std::string contentType;
        std::string location;

        response();
};

class location {
    public:
        vector_map                                          directives;

        std::map<std::string, std::string>                  cgis; 
        std::map<std::string, std::string>                  mime_types;
        std::pair<std::string, std::string>                 ipPort;
        int                                                 socket;
        std::ifstream                                       locationFile;
        int                                                 code;
        std::string                                         path;
};

class Client {
    public:
        std::map<std::string, std::string>                      request;
        std::string                                             body;
        std::string                                             respond;
        location *                                              c_location;
        std::map<std::string, std::string>                      env_variables;
        std::map<int, std::pair<std::string, std::string> >     error_pages;
        response                                                res;
        std::vector<std::string>                                cookies;
    
        bool                                                    isFirstTime;
        bool                                                    Respond;
        bool                                                    Request;
        bool                                                    readIsAllowed;
        bool                                                    isChunck;
        bool                                                    hasCgi;
        bool                                                    isReadAllowed;

        std::size_t                                             currentChunkSize;
        std::size_t                                             contentLength;
        std::size_t                                             bodySize;
        std::ofstream                                           objFile;
        std::string                                             postFile;
        bool                                                    dataUploaded;
        std::string                                             boundaryType;
        bool                                                    boundaryStarts;
        bool                                                    boundaryLength;
        std::size_t                                             readBytes;

        pid_t                                                   cgiProcessId;
        std::string                                             cgiOutputFile;
        clock_t                                                 cgiTimeStart;
        bool                                                    isCgiInProcess;
        std::string                                             root;
        std::vector<std::string>                                files;
        struct timeval                                          startTime;
        struct timeval                                          postStartTime;

        std::string                                             locationName;
        

        Client();
        ~Client();

        void    check_request(Data * data, int socket);
        void    fillLocation(Data * data, int socket, std::string & path);

};

class server {
    public:
        vector_map                                          directives;
        std::map<std::string, location*>                    locations;
        std::map<std::string, std::string>                  mime_types;
        std::map<std::string, std::string>                  cgis;
        std::map<int , std::string>                         error_pages; 
};

#endif