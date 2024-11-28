#include "../includes/get.hpp"

std::string Get::getDires() {
    DIR * dir = opendir(client->c_location->directives["root"][0].c_str());
    std::vector<std::string> content_list;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
        content_list.push_back(entry->d_name);
    std::string text = "<!DOCTYPE html>\n<html>\n<head>\n<title>webserver</title>\n</head>\n<body>\n";
    std::vector<std::string>::iterator it = content_list.begin();
    for(; it != content_list.end(); it++) {
        if(client->request["path"][client->request["path"].size() - 1] != '/')
            text += "<a href=\"" + client->request["path"] + "/" + *it + "\">" + *it + "</a>\n<hr>";
        else
            text += "<a href=\"" + client->request["path"] + *it + "\">" + *it + "</a>\n<hr>";
    }
    text += "</body>\n</html>";
    closedir(dir);
    return text;
}

Get::Get(Client * client) {
    this->client = client;
    this->request_content =  client->c_location ->directives["root"][0];
}

void Get::getFileResponse(std::string extention) {
    char * fileContent = readChuncks(client, client->c_location->locationFile);
    if(client->hasCgi) {
        fillGetResponse(client, client->res.code, "text/html", fileContent, "", client->c_location->locationFile.gcount());
    } else {
        if(client->res.code == -1)
            client->res.code = 200;
        fillGetResponse(client, client->res.code, getMimeType(client->c_location->mime_types, extention), fileContent, "", client->c_location->locationFile.gcount());
    }
    delete [] fileContent;
}

bool Get::isFileIndexExist(std::string & extention, std::string & file_path) {
    std::vector<std::string>::iterator it = client->c_location->directives["index"].begin();
    for(; it != client->c_location->directives["index"].end(); it++) {
        file_path = client->c_location->directives["root"][0] + *it;
        if(access(file_path.c_str(), F_OK) == 0 && access(file_path.c_str(), R_OK) == 0 )
        {
            extention = getExtension(file_path);
            return true;
        }
    }
    return false;
}

void Get::autoIndexOff() {
    std::string extention, file_path;
    bool isFileExist = isFileIndexExist(extention, file_path);
    if(!isFileExist || isDirectory(file_path)) {
        fillGetResponse(client, 403, "", "", "", 0);
        client->Respond = true;
    } else if(hasCgi(extention, client->c_location->cgis)) {
        client->c_location->directives["root"][0] = file_path;
        saveEnvirements(client);
        cgi(client, extention);
        getFileResponse(extention);
    }
    else {
        if(client->isFirstTime)
            client->c_location->locationFile.open(file_path.c_str());
        getFileResponse(extention);
        client->Respond = true;
    }
}

void Get::cgi(Client * client, std::string extention) {
    client->hasCgi = true;
    if(!client->isChunck)
        runCGI(extention, client);
}

void Get::getRequestedFile() {
    std::string extention = getExtension(request_content);
    if(client->isFirstTime) {
        if(hasCgi(extention, client->c_location->cgis))
            cgi(client, extention);
        else {
            if(!client->c_location->locationFile.is_open())
                client->c_location->locationFile.open(request_content.c_str());
            client->Respond = true;
        }
    }
    if(client->Respond)
        getFileResponse(extention);
}

std::string Get::getExistedIndex() {
    std::vector<std::string>::iterator it = client->c_location->directives["index"].begin();
    for(; it != client->c_location->directives["index"].end(); it++) {
        std::string file_path = client->c_location->directives["root"][0] + *it;
        if(access(file_path.c_str(), F_OK) == 0 && access(file_path.c_str(), R_OK) == 0)
            return *it;
    }
    return "";
}

void Get::getRequestedDir() {
    std::string extention, file_path;
    if(request_content[request_content.size() - 1] != '/')
        fillGetResponse(client,  301, "", "", (client->request["url"] + "/").c_str() , 0);
    else {
        if(client->c_location->directives["autoindex"][0] == "on") {
            if(isFileIndexExist(extention, file_path)) {
                if(isDirectory(file_path)) {
                    fillGetResponse(client,  301, "", "", getExistedIndex(), 0);
                } else if(hasCgi(extention, client->c_location->cgis)) {
                    client->c_location->directives["root"][0] = file_path;
                    saveEnvirements(client);
                    cgi(client, extention);
                } else {
                    if(client->isFirstTime)
                        client->c_location->locationFile.open(file_path.c_str());
                    client->Respond = true;
                }
                if(client->Respond && client->res.code != 301)
                    getFileResponse(extention);
            } else
                fillGetResponse(client, 200, "text/html", getDires().c_str(), "", getDires().size());
        }
        else
            autoIndexOff();
    }
}

void Get::getResponse() {
    !isDirectory(request_content) ? getRequestedFile() : getRequestedDir();
}