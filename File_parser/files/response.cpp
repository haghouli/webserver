#include "../includes/response.hpp"

std::string strToLower(std::string str) {
    std::string s;
    for(size_t i = 0; i < str.size(); i++)
        s += tolower(str[i]);
    return s;
}

std::string getCurrentTime() {
    time_t currentTime;
    struct tm * timeinfo;
    char buffer[100];

    time(&currentTime);
    timeinfo = localtime(&currentTime);

    strftime(buffer, 100, "%a, %d %b %G %H:%M:%S GMT", timeinfo);
    return buffer;
}

void getDefaultErrorPage(Client * client) {

    std::string defaultErrorPage = "<!DOCTYPE html>\
    <html style=\"height:100%\"><head>\
    <meta name=\"viewport\" content=\"width=device-width, \
    initial-scale=1, shrink-to-fit=no\" />\
    <title>" + NumberToString(client->res.code) + " " + client->error_pages[client->res.code].first + "</title></head> \
    <body style=\"color: #444; margin:0;font: normal 14px/20px Arial,\
    Helvetica, sans-serif; height:100%; background-color: #fff;\"> \
    <div style=\"height:auto; min-height:100%; \">\
    <div style=\"text-align: center; width:800px; margin-left: -400px; \
    position:absolute; top: 30%; left:50%;\"> \
    <h1 style=\"margin:0; font-size:150px; \
    line-height:150px; font-weight:bold;\">" + NumberToString(client->res.code)  + "</h1> \
    <h2 style=\"margin-top:20px;font-size: 30px;\">\
    " + client->error_pages[client->res.code].first + "</h2> \
    <p>Error occured during the server work.</p> \
    </div></div></body></html>";

    fillGetResponse(client, client->res.code, "text/html", defaultErrorPage.c_str(), "", defaultErrorPage.size());
}

void    erorrResponse(Client * client) {
    std::string fileContent;
    int code = client->res.code;
    std::map<int, std::pair<std::string, std::string> > errorPages = client->error_pages; 
    
    if(errorPages[code].second.empty()) {
        getDefaultErrorPage(client);
    } else {
        if(client->isFirstTime)
            client->c_location->locationFile.open(errorPages[code].second.c_str());
        if(!client->isChunck && (!client->c_location->locationFile || isDirectory(errorPages[code].second))) {
            getDefaultErrorPage(client);
        }
        else {
            char * fileContent = readChuncks(client, client->c_location->locationFile);
            if(client->isFirstTime) {
                std::string type;
                std::string extention = getExtension(errorPages[code].second);
                if(client->request["method"] == "POST")
                    type = getMimeType(swapMap(client->c_location->mime_types), extention);
                else
                    type = getMimeType(client->c_location->mime_types, extention);
                fillGetResponse(client, client->res.code, type, fileContent, "", client->c_location->locationFile.gcount());
            } else
                fillGetResponse(client, client->res.code, "", fileContent, "", client->c_location->locationFile.gcount());
            delete [] fileContent;
        }
    }
    client->Respond = true;
}

std::string getTextCode(Client * client) {
    return client->error_pages[client->res.code].first;
}

bool    hasStr(std::string content, std::string toFind) {
    size_t pos = content.find(toFind);
    return pos == std::string::npos ? false : true;
}

std::map<std::string, std::string> parseHeader(std::string header, std::vector<std::string> & cookies) {
    std::map<std::string, std::string> headerElements;
    std::vector<std::string> v1 = splite(header, "\n");
    std::vector<std::string>::iterator it = v1.begin();
    for(; it != v1.end(); it++) {
        size_t pos = it->find(":");
        if(pos != std::string::npos) {
            std::string key = strToLower(it->substr(0, pos));
            std::string value = it->substr(pos + 1);
            if(key == "set-cookie")
                cookies.push_back(value);
            else
                headerElements[strTrim(key, " \t\n\r")] = strTrim(value, " \t\n\r");
        }
    }
    return headerElements;
}

void getHeaderElements(std::map<std::string, std::string> & headerElements, response & res) {
    if(headerElements.size() != 0) {
        if(headerElements.find("status") != headerElements.end()) {
            res.code = atoi(headerElements["status"].substr(0, 3).c_str());
            headerElements.erase("status");
        }
        if(headerElements.find("content-type") != headerElements.end()) {
            res.contentType = headerElements["content-type"];
            headerElements.erase("content-type");
        }
        else
            res.contentType = "text/html";
        if(headerElements.find("content-length") != headerElements.end())
            headerElements.erase("content-length");
        res.contentSize = res.content.size();
    } else {
        res.contentType = "text/html";
    }
}

void    saveResponse(Client * client, std::map<std::string, std::string> headerElements, std::vector<std::string> cookies) {
    std::string textCode = getTextCode(client);
    client->respond += "HTTP/1.1 " + NumberToString(client->res.code) + " " + textCode + "\r\n";
    client->respond += "Access-Control-Allow-Origin: *\r\n";
    client->respond += "Server: webserver\r\n";
    client->respond += "Date: " + getCurrentTime() + "\r\n";
    if(client->res.code >= 300 &&  client->res.code < 400 && !client->hasCgi)
        client->respond += "Location: " + client->res.location + "\r\n\r\n";
    else {
        std::map<std::string, std::string>::iterator it = headerElements.begin();
        for(; it != headerElements.end(); it++)
            client->respond += it->first + ": " + it->second + "\r\n";
        std::vector<std::string>::iterator it1 = cookies.begin();
        for(; it1 != cookies.end(); it1++)
            client->respond +=  "Set-Cookie: " + *it1 + "\r\n";
        client->respond += "Content-Type: " + client->res.contentType + "\r\n";
        if(!client->isChunck) {
            client->respond += "Content-Length:" + NumberToString(client->res.contentSize) + "\r\n\r\n";
        } else {
            client->respond += "Transfer-Encoding: chunked\r\n\r\n";
            std::string ckunked_tag = intToHex(client->res.contentSize) + "\r\n";
            client->respond.append(ckunked_tag, 0, ckunked_tag.size());
        }
        client->respond.append(client->res.content, 0, client->res.contentSize);
        client->respond += "\r\n";
    }
}

void ResponseGenerator::getResponse(Client * client) {
    client->respond.clear();
    if(!client->hasCgi && client->res.code >= 400) {
        erorrResponse(client);
    } if(!client->Respond)
        return ;
    if(client->isFirstTime) {
        std::string content, header;
        std::map<std::string, std::string> headerElements;
        std::vector<std::string> cookies;
        if(client->hasCgi) {
            if(hasStr(client->res.content, "\r\n\r\n")) {
                header = client->res.content.substr(0, client->res.content.find("\r\n\r\n") + 1);
                client->res.content = client->res.content.substr(client->res.content.find("\r\n\r\n") + 4);
                headerElements = parseHeader(header, cookies);
            }
            getHeaderElements(headerElements, client->res);
        }
        saveResponse(client, headerElements, cookies);
    } else if (client->isChunck) {
        std::string ckunked_tag = intToHex(client->res.contentSize) + "\r\n";
        client->respond.append(ckunked_tag, 0, ckunked_tag.size());
        client->respond.append(client->res.content, 0, client->res.contentSize);
        client->respond += "\r\n";
    } else {
        client->respond += "0\r\n\r\n";
    }
}