#include "../File_parser/includes/header.hpp"
#include <sstream>

Client::Client(std::string &str)
{
    std::cout << "req cons has been called\n" << std::endl;
    std::size_t requestLineEnd = str.find("\r\n");
    if (requestLineEnd == std::string::npos) 
    {
        //error
    }

    std::string requestLine = str.substr(0, requestLineEnd);
    
    std::istringstream requestLineStream(requestLine);
    std::string method, url, version;
    requestLineStream >> method >> url >> version;

    request["method"] = method;
    request["url"] = url;
    request["version"] = version;

    std::size_t headersStart = requestLineEnd + 2; // Move past "\r\n"

    std::size_t headersEnd = str.find("\r\n\r\n", headersStart);
    if (headersEnd == std::string::npos) 
    {
        //return request; error
    }

    std::string headers = str.substr(headersStart, headersEnd - headersStart);

    std::istringstream headersStream(headers);
    std::string header;
    while (std::getline(headersStream, header)) 
    {
        std::size_t pos = header.find(":");
        if (pos != std::string::npos) {
            std::string key = header.substr(0, pos);
            std::string value = header.substr(pos + 1);
            request[key] = value;
        }
    }

    std::size_t bodyStart = headersEnd + 4; // Move past "\r\n\r\n"

    std::string body = str.substr(bodyStart);
    request["body"] = body;
}

Client::~Client()
{
    std::cout << "req des has been called\n" << std::endl;
    // std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
    //  std::map<std::string, std::string>::iterator it = request.begin();
    //  while (it != request.end()){
    //     std::cout << "key: " << it->first << "  value: " << it->second << std::endl;
    //     it++;
    //  }
    // std::map<std::string, std::string>::iterator it = request.lower_bound("\nUser-Agent");
    // std::cout << "->key: " << it->first << "  ->value: " << it->second << std::endl;
}