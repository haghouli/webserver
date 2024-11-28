#include "../includes/Post.hpp"

size_t hexaToDic(std::string &hexString)
{
    std::stringstream ss;
    ss << std::hex << hexString;
    size_t decimalValue;
    ss >> decimalValue;
    return (decimalValue);
}

bool  getPath(Client &currentClient)
{
    if(currentClient.c_location->directives["uploads"][0] == "off")
    {
        currentClient.res.code = 403;
        currentClient.Respond = true;
        return (false);
    }
    else
        currentClient.c_location->path = currentClient.c_location->directives["uploads"][1];
    return true;
}

std::string generateFileName(Client &currentClient, std::string contentType)
{
    if (!currentClient.objFile.is_open())
    {
        std::string path = currentClient.c_location->path;
        std::map<std::string, std::string>::iterator it = currentClient.c_location->mime_types.find(contentType);
        int index = 0;
        std::string cgiExtention = getExtension(currentClient.c_location->directives["root"][0]);
        if (!hasCgi(cgiExtention, currentClient.c_location->cgis))
        {
            if (it == currentClient.c_location->mime_types.end()) {
                currentClient.res.code = 415;
                currentClient.Respond = true;
                return ("");
            }
        }
        std::string extention = currentClient.c_location->mime_types[contentType];
        currentClient.postFile = path + "/" + "content_00." + extention;
        std::string x_str ;
        while (!access((currentClient.postFile).c_str(), F_OK ))
        {
            std::stringstream ss;
            index ++;
            ss << index;
            std::string x_str = ss.str();
            currentClient.postFile  = path + "/" +  "content_0" + x_str + "." + extention;
        }
    }
    return (currentClient.postFile);
}

std::size_t  getFile(Client &currentClient, std::string contentType)
{
    std::string fileName = generateFileName(currentClient, contentType);
    if (!currentClient.objFile.is_open())
    {
        if (fileName.empty())
            return (0);
        currentClient.objFile.open(fileName.c_str(), std::ios::app | std::ios::binary);
        
        if(!currentClient.objFile.is_open())
        {
            currentClient.res.code = 500;
            currentClient.Respond = true;
            return (0);
        }
    }
    return (1);
}

double bytesToMegabytes(size_t bytes) {
    return static_cast<double>(bytes) / (1024 * 1024);
}

bool checkLength(Client &currentClient, double contentSize)
{
    double clientMaxSize = strToDouble(currentClient.c_location->directives["client_max_body_size"][0]);
    if (clientMaxSize < bytesToMegabytes(contentSize))
    {
        currentClient.Respond = true;
        currentClient.res.code = 413;
        if ( currentClient.objFile)
            currentClient.objFile.close();
        std::remove(currentClient.postFile.c_str());
        return (0);
    }
    return (1);
}

void removeCRLF(Client &currentClient)
{
    if (currentClient.body.substr(0,2) == "\r\n")
    {
        currentClient.bodySize -= 2;
        currentClient.contentLength -= 2;
        currentClient.body = currentClient.body.substr(2, currentClient.bodySize );
    }
}

void checkPermitions(Client &currentClient)
{
    if (access(currentClient.postFile.c_str(), W_OK | F_OK) != 0)
    {
        if (!access(currentClient.postFile.c_str(),  F_OK))
        {
            if(currentClient.objFile.is_open())
                currentClient.objFile.close();
            std::remove(currentClient.postFile.c_str());
            std::cerr << "Permesion denid" << std::endl;
        }
        else
            std::cerr << "Can't find file to post" << std::endl;
        currentClient.Respond = true;
        currentClient.res.code = 500;
    }
}