#include "../../File_parser/includes/utiles.hpp"

response::response() {
    code = -1;
    contentSize = 0;
}

Client::Client()
{
    hasCgi              = false;
    Request             = false;
    Respond             = false;
    isChunck            = false;
    isFirstTime         = true;
    readIsAllowed       = true;
    cgiProcessId        = -1;
    bodySize            = 0;
    readIsAllowed       = true;
    currentChunkSize    = 0;
    contentLength       = 0;
    readBytes           = 0;
    isCgiInProcess      = false;
    dataUploaded        = false;
    boundaryStarts      = false;
    boundaryLength      = 0;
    c_location          = new location();
    isReadAllowed       = true;
    gettimeofday(&startTime, NULL);
}

void Client::fillLocation(Data *data, int socket, std::string &path)
{
    std::pair<std::string, std::string> ipPort = data->getIpPorts()[socket];
    std::string server_name = request["Host"];
    server *s = data->findServer(ipPort, server_name);
    std::string url = request["path"];

    c_location->mime_types = s->mime_types;
    c_location->ipPort = data->getIpPorts()[socket];
    c_location->socket = socket;

    if (hasLocation(url, s->locations, path))
    {
        c_location->directives = s->locations[path]->directives;
        !s->locations[path]->cgis.empty() ? c_location->cgis = s->locations[path]->cgis : c_location->cgis = s->cgis;
    }
    else
    {
        c_location->directives = s->directives;
        c_location->cgis = s->cgis;
    }
    vector_map::iterator it = s->directives.begin();
    for (; it != s->directives.end(); it++)
    {
        if (c_location->directives.find(it->first) == c_location->directives.end() && it->first != "server_name" && it->first != "listen" && it->first != "return")
        {
            if (it->first == "root" && c_location->directives.find("alias") != c_location->directives.end())
                continue;
            else
            {
                std::vector<std::string>::iterator it1 = it->second.begin();
                for (; it1 != it->second.end(); it1++)
                    c_location->directives[it->first].push_back(*it1);
            }
        }
    }
    if(this->c_location->directives.find("alias") != this->c_location->directives.end())
        this->locationName = path;
}

void fixPaths(location * l) {
    if(l->directives.find("root") != l->directives.end())
        l->directives["root"][0] = getRealPath(l->directives["root"][0].c_str());
    if(l->directives.find("alias") != l->directives.end())
        l->directives["alias"][0] = getRealPath(l->directives["alias"][0].c_str());
}

void Client::check_request(Data *data, int socket)
{
    std::string server_name = request["Host"];
    std::string url = request["path"];
    std::string method = request["method"];

    std::string path;
    fillLocation(data, socket, path);
    fixPaths(this->c_location);
    if(c_location->directives.find("root") != c_location->directives.end())
        this->root = c_location->directives["root"][0];
    else
        this->root = c_location->directives["alias"][0];
    getAbsolutePath(c_location->directives, url, path);
    saveEnvirements(this);

    std::string realPath = getRealPath(c_location->directives["root"][0].c_str());

    if (access(c_location->directives["root"][0].c_str(), F_OK) != 0)
        this->res.code = 404;
    else if (access(c_location->directives["root"][0].c_str(), R_OK) != 0)
        this->res.code = 403;
    else if(!isSubdirectory(this->root, realPath)) {
        this->res.code = 400;
    }
    else if (find(c_location->directives["allow_methods"].begin(), c_location->directives["allow_methods"].end(), method) == c_location->directives["allow_methods"].end())
        this->res.code = 405;
    else if (c_location->directives.find("return") != c_location->directives.end())
        fillGetResponse(this, atoi(c_location->directives["return"][0].c_str()), "", "", c_location->directives["return"][1].c_str(), 0);
    else
        this->res.code = 200;
    if(method == "POST")
        this->c_location->mime_types = swapMap(this->c_location->mime_types);
    this->c_location->directives["client_max_body_size"][0].erase(this->c_location->directives["client_max_body_size"][0].size() - 1, 1);
    this->c_location->directives["request_cgi_timeout"][0].erase(this->c_location->directives["request_cgi_timeout"][0].size() - 1, 1);
}

Client::~Client() {
    delete c_location;
}