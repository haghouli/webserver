#include "../includes/Post.hpp"

std::string getValue(std::size_t start, std::string str)
{
    std::size_t posR = str.find("\r", start);
    std::size_t posN = str.find("\n", start);
    if (posR != std::string::npos || posN != std::string::npos)
    {
        return posR < posN ? str.substr(start, posR - start)
                            :  str.substr(start, posN);
    }
    return "";
}

void  parceHeader(Client &currentClient, std::string header)
{
    std::size_t pos = header.find("Content-Type: ");
    if (pos != std::string::npos)
    {
        pos += std::string("Content-Type: ").size();
        currentClient.boundaryType =  getValue(pos, header);
    }
    else
        currentClient.boundaryType =  "text/plain";
}

bool parcePart(Client &currentClient, std::string &chunk)
{
    std::size_t pos = chunk.find("\r\n\r\n");
    std::string header;
    if (pos != std::string::npos)
    {
        header = chunk.substr(0, pos + 4);
        chunk.erase(0, pos + 4);
        parceHeader(currentClient, header);
        currentClient.bodySize -= (pos + 4);
        return (true);                                                                  
    }
    return (false);
}
