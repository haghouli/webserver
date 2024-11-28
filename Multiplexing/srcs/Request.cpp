#include "../includes/Multi.hpp"

std::string oneSlashe(const std::string& str) {
    std::string result;
    bool isFirstSlash = true;

    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '/') {
            if (isFirstSlash) {
                result += '/';
                isFirstSlash = false;
            }
        } else {
            result += str[i];
            isFirstSlash = true;
        }
    }
    return result;
}

bool isAllowedChar(char c) {
    std::string str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+=%,; ";
    size_t found = str.find(c);
    if (found == std::string::npos)
        return false;
    return true;
}

bool isValidUrl(const char* url) {
    for (std::size_t i = 0; i < std::strlen(url); ++i) {
        if (!isAllowedChar(url[i])) {
            return false;
        }
    }
    return true;
}

int checkUrl(std::string &url, Client* client)
{
    if (url.length() > 2048) {
        client->Respond = true;
        return 414;
    }
    if (!isValidUrl(url.c_str()))
    {
        client->Respond = true;
        return 400;
    }
    return 200;
}

void extractQueryAndFragment(const std::string& uri, Client *client) {
    size_t queryPos = uri.find('?');
    size_t fragmentPos = uri.find('#');

    client->request["path"] = uri.substr(0, queryPos);
    if (queryPos != std::string::npos) {
        client->request["query"] = uri.substr(queryPos + 1, fragmentPos - queryPos - 1);
    }
    if (fragmentPos != std::string::npos) {
        client->request["fragment"] = uri.substr(fragmentPos + 1);
    }
}

 
std::string decodeURI(const std::string& uri) {
    std::ostringstream decoded;
    for (std::string::const_iterator it = uri.begin(); it != uri.end(); ++it) {
        if (*it == '%') {
            int distance = std::distance(it, uri.end());
            std::string hexValue;
            if (distance >= 3) {
                hexValue = std::string(it + 1, it + 3);
                it += 2;
            } else if (distance >= 2){
                hexValue= std::string(it + 1, it + 3);
                it += 1;
            }
            else  return "\n";
            std::istringstream hexStream(hexValue);
            int charCode;
            hexStream >> std::hex >> charCode;
            if (charCode == 0) return "\n";
            decoded << static_cast<char>(charCode);
        } else  decoded << *it;
    }
    return decoded.str();
}

void trimString(std::string &key, std::string &value)
{
    key = trim(key);
    value = trim(value);
    if (key == "Host")
    {
        size_t pos = value.find(":");
        if (pos != std::string::npos) {
            value = value.substr(0, pos);
        }
    }
}

bool boundryIsExist(Client *client)
{
    std::string data  = client->request["Content-Type"];
    size_t pos = data.find("multipart/form-data");
    if (pos == std::string::npos) 
        return false;
    return true;
}

bool methodCheck(const std::string &method)
{
    if (method != "GET" && method != "POST" && method != "DELETE")
        return false;
    return true;
}

int parseRequestAndHeaders(Client* client) {
    bool transferEncodingFound = false;
    bool contentLenghtFound  = false;
    std::size_t requestLineEnd = client->body.find("\r\n");
    std::string requestLine = client->body.substr(0, requestLineEnd);
    std::istringstream requestLineStream(requestLine);
    std::string method, url, version;
    requestLineStream >> method >> url >> version;
    if (!methodCheck(method) || url.empty() || url[0] != '/')
        return 400;
    client->request["method"] = method;
    url = decodeURI(url);
    client->request["url"] = oneSlashe(url);
    int statusCode = checkUrl(client->request["url"], client);
    if (statusCode != 200) 
        return statusCode;
    extractQueryAndFragment(client->request["url"], client);
    client->request["version"] = version;
    std::size_t headersStart = requestLineEnd + 2;
    std::size_t headersEnd = client->body.find("\r\n\r\n");
    if (headersStart - 2 == headersEnd)
        return 400;
    std::string headers = client->body.substr(headersStart, headersEnd - headersStart);
    std::istringstream headersStream(headers);
    std::string header;
    while (std::getline(headersStream, header)) {
        std::size_t pos = header.find(":");
        if (pos != std::string::npos) {
            std::string key = header.substr(0, pos);
            std::string value = header.substr(pos + 1);
            trimString(key, value);
            if (key != "Cookie")
                client->request[key] = value;
            if (key == "Transfer-Encoding") {
                transferEncodingFound = true;
                if (value != "chunked")
                    return 501;
            }
            if (key == "Content-Length")
                contentLenghtFound = true;
            if (key == "Cookie")
                client->cookies.push_back(value);
        }
    }
    if (client->request["method"] == "POST" && client->request["Content-Type"].empty())
        return 400;
    if (client->request["Host"].empty() || (transferEncodingFound && boundryIsExist(client)))
        return 400;
    if (client->request["method"] == "POST" && !transferEncodingFound && !contentLenghtFound 
    && !boundryIsExist(client))
        return 411;
    client->body = client->body.substr(headersEnd + 4);
    client->Request = true;
    return 200;
}