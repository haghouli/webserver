#include "../includes/utiles.hpp"

std::string getRealPath(const char * path) {
    char str[PATH_MAX];
    realpath(path, str);
    return str;
}

std::string & strTrim(std::string& input, std::string s)
{ 
    input.erase(0, input.find_first_not_of(s));
    input.erase(input.find_last_not_of(s) + 1);
    return input;
}

bool isDouble(const std::string & s) {
    std::istringstream iss(s);
    double val;
    iss >> std::noskipws >> val;

    return iss.eof() && !iss.fail();
}

std::string getMimeType(std::map<std::string, std::string> mime_types, std::string extention) {
    std::map<std::string, std::string>::iterator it = mime_types.find(extention);
    if(it != mime_types.end())
        return it->second;
    if(extention == "html")
        return "text/html";
    return "text/plain";
}

void    fillGetResponse(Client * client, int code, std::string type,  const char * content, std::string l, int content_size) {
    client->Respond = true;
    client->res.content.clear();
    client->res.code = code;
    client->res.contentType = type;
    client->res.content.append(content, content_size);
    client->res.location = l;
    client->res.contentSize = content_size;
}

char *    readChuncks(Client * client, std::ifstream & file) {
    
    size_t bufferSize = 4096;
    char * buffer = new char[bufferSize + 1]();
    if(file.tellg() == -1) {
        file.close();
        client->isChunck = false;
        std::remove(client->cgiOutputFile.c_str());
        return buffer;
    }

    file.read(buffer, bufferSize);
    if(file.eof() && client->isFirstTime) {
        file.close();
        std::remove(client->cgiOutputFile.c_str());
        client->isChunck = false;
    } else {
        client->isChunck = true;
    }
    return buffer;
}

std::string &trim(std::string& input)
{ 
    input.erase(0, input.find_first_not_of(" \t\n\r"));
    input.erase(input.find_last_not_of(" \t\n\r") + 1);
    return input;
}

double  convertToSeconds(std::string str) {
    std::string suffix = str.substr(str.size() - 1, 1);
    double value = strToDouble(str.substr(0, str.size() - 1));

    if(suffix == "h") 
        return (value * pow(60 , 2));
    else if(suffix == "m")
        return (value * 60);
    return value;
}

std::string getFileRoot(std::string path) {
    size_t pos = path.rfind("/");
    if(pos != std::string::npos)
        return path.substr(0, pos + 1);
    return "";
}

std::string getExtension(std::string path) {
    std::string extention;
    size_t idx = path.find_last_of('.');
    if(idx != std::string::npos)
        extention = path.substr(idx + 1);
    return extention;
}

bool hasCgi(std::string extention, std::map<std::string, std::string> cgisMap) {
    std::map<std::string, std::string>::iterator it = cgisMap.find(extention);
    if(it != cgisMap.end())
        return true;
    return false;
}

std::string generateRandomFile(std::string filePrefix, std::string root) {
    std::vector<std::string> rootFiles;
    DIR * dir = opendir(root.c_str());
    struct dirent* entry;
    
    while ((entry = readdir(dir)) != NULL)
        rootFiles.push_back(entry->d_name);
    closedir(dir);
    for(size_t i = 0; i < 10000; i++) {
        std::string fileName = filePrefix + NumberToString(i);
        if(std::find(rootFiles.begin(), rootFiles.end(), fileName) == rootFiles.end())
            return fileName;
    }
    return "";
}

double strToDouble(std::string str) {
    std::istringstream iss(str);
    double myDouble;
    iss >> myDouble;
    return myDouble;
}

double  convetToMb(std::string str) {
    std::string suffix = str.substr(str.size() - 1, 1);
    double value = strToDouble(str.substr(0, str.size() - 1));
    if(suffix == "M")
        return value    ;
    else if(suffix == "K")
        return value / 1024;
    return value * 1024;
}

bool isSubdirectory(std::string root, std::string subDire) {
    if(root[root.size() - 1] != '/')
        root += "/";
    if(subDire[subDire.size() - 1] != '/')
        subDire += "/";
    
    if(root[0] != '/')
        root = "/" + root;
    if(subDire[0] != '/')
        subDire = "/" + subDire;

    if(subDire.size() < root.size())
        return false;
    if(subDire.substr(0, root.size()) != root)
        return false;
    return true;
}

std::string getClairPath(std::string subDire) {
    std::vector<std::string> v = splite(subDire, "/");
    std::string fixedPath;

    while(1) {
        std::vector<std::string>::iterator it = v.begin();
        for(; it != v.end(); it++) {
            if(*it == "..") {
                v.erase(it);
                if(it != v.begin())
                    v.erase(it - 1);
                break;
            }
        }
        if(std::find(v.begin(), v.end(), "..") == v.end())
            break;
    }
    if(!v.empty()){
        for(size_t i = 0; i < v.size(); i++)
            fixedPath += v[i] + "/";
    } else
        fixedPath = "/";
    return fixedPath;
}

void fillErrorPages(Client * client, server * s) {
    std::map<int, std::string> httpStatusCodes;

    httpStatusCodes[200] = "OK";
    httpStatusCodes[201] = "Created";
    httpStatusCodes[202] = "Accepted";
    httpStatusCodes[204] = "No Content";
    httpStatusCodes[206] = "Partial Content";

    httpStatusCodes[300] = "Multiple Choices";
    httpStatusCodes[301] = "Moved Permanently";
    httpStatusCodes[302] = "Found (Temporary Redirect)";
    httpStatusCodes[304] = "Not Modified";
    httpStatusCodes[307] = "Temporary Redirect";
    httpStatusCodes[308] = "Permanent Redirect";

    httpStatusCodes[400] = "Bad Request";
    httpStatusCodes[401] = "Unauthorized";
    httpStatusCodes[402] = "Payment Required";
    httpStatusCodes[403] = "Forbidden";
    httpStatusCodes[404] = "Not Found";
    httpStatusCodes[405] = "Method Not Allowed";
    httpStatusCodes[406] = "Not Acceptable";
    httpStatusCodes[407] = "Proxy Authentication Required";
    httpStatusCodes[408] = "Request Timeout";
    httpStatusCodes[409] = "Conflict";
    httpStatusCodes[410] = "Gone";
    httpStatusCodes[411] = "Length Required";
    httpStatusCodes[412] = "Precondition Failed";
    httpStatusCodes[413] = "Payload Too Large";
    httpStatusCodes[414] = "URI Too Long";
    httpStatusCodes[415] = "Unsupported Media Type";
    httpStatusCodes[416] = "Range Not Satisfiable";
    httpStatusCodes[417] = "Expectation Failed";
    httpStatusCodes[418] = "I'm a teapot";
    httpStatusCodes[421] = "Misdirected Request";
    httpStatusCodes[422] = "Unprocessable Entity";
    httpStatusCodes[423] = "Locked";
    httpStatusCodes[424] = "Failed Dependency";
    httpStatusCodes[426] = "Upgrade Required";
    httpStatusCodes[428] = "Precondition Required";
    httpStatusCodes[429] = "Too Many Requests";
    httpStatusCodes[431] = "Request Header Fields Too Large";
    httpStatusCodes[451] = "Unavailable For Legal Reasons";

    httpStatusCodes[500] = "Internal Server Error";
    httpStatusCodes[501] = "Not Implemented";
    httpStatusCodes[502] = "Bad Gateway";
    httpStatusCodes[503] = "Service Unavailable";
    httpStatusCodes[504] = "Gateway Timeout";
    httpStatusCodes[505] = "HTTP Version Not Supported";
    httpStatusCodes[506] = "Variant Also Negotiates";
    httpStatusCodes[507] = "Insufficient Storage";
    httpStatusCodes[508] = "Loop Detected";
    httpStatusCodes[510] = "Not Extended";
    httpStatusCodes[511] = "Network Authentication Required";

    std::map<int, std::string>::iterator it = httpStatusCodes.begin();
    for(; it != httpStatusCodes.end(); it++) {
        client->error_pages[it->first].first = it->second; 
        if(s->error_pages.find(it->first) != s->error_pages.end())
            client->error_pages[it->first].second = s->error_pages[it->first];
    }
}

std::map<std::string, std::string> getAllMimeTypes() {
    std::map<std::string, std::string> mimeTypes;

    mimeTypes["html"] = "text/html";
    mimeTypes["htm"] = "text/html";
    mimeTypes["css"] = "text/css";
    mimeTypes["xml"] = "text/xml";
    mimeTypes["rss"] = "text/xml";
    mimeTypes["gif"] = "image/gif";
    mimeTypes["jpeg"] = "image/jpeg";
    mimeTypes["jpg"] = "image/jpeg";
    mimeTypes["js"] = "application/x-javascript";
    mimeTypes["txt"] = "text/plain";
    mimeTypes["htc"] = "text/x-component";
    mimeTypes["mml"] = "text/mathml";
    mimeTypes["png"] = "image/png";
    mimeTypes["ico"] = "image/x-icon";
    mimeTypes["jng"] = "image/x-jng";
    mimeTypes["wbmp"] = "image/vnd.wap.wbmp";
    mimeTypes["jar"] = "application/java-archive";
    mimeTypes["war"] = "application/java-archive";
    mimeTypes["ear"] = "application/java-archive";
    mimeTypes["hqx"] = "application/mac-binhex40";
    mimeTypes["pdf"] = "application/pdf";
    mimeTypes["cco"] = "application/x-cocoa";
    mimeTypes["jardiff"] = "application/x-java-archive-diff";
    mimeTypes["jnlp"] = "application/x-java-jnlp-file";
    mimeTypes["run"] = "application/x-makeself";
    mimeTypes["pl"] = "application/x-perl";
    mimeTypes["pm"] = "application/x-perl";
    mimeTypes["prc"] = "application/x-pilot";
    mimeTypes["pdb"] = "application/x-pilot";
    mimeTypes["rar"] = "application/x-rar-compressed";
    mimeTypes["rpm"] = "application/x-redhat-package-manager";
    mimeTypes["sea"] = "application/x-sea";
    mimeTypes["swf"] = "application/x-shockwave-flash";
    mimeTypes["sit"] = "application/x-stuffit";
    mimeTypes["tcl"] = "application/x-tcl";
    mimeTypes["tk"] = "application/x-tcl";
    mimeTypes["der"] = "application/x-x509-ca-cert";
    mimeTypes["pem"] = "application/x-x509-ca-cert";
    mimeTypes["crt"] = "application/x-x509-ca-cert";
    mimeTypes["xpi"] = "application/x-xpinstall";
    mimeTypes["zip"] = "application/zip";
    mimeTypes["deb"] = "application/octet-stream";
    mimeTypes["bin"] = "application/octet-stream";
    mimeTypes["exe"] = "application/octet-stream";
    mimeTypes["dll"] = "application/octet-stream";
    mimeTypes["dmg"] = "application/octet-stream";
    mimeTypes["eot"] = "application/octet-stream";
    mimeTypes["iso"] = "application/octet-stream";
    mimeTypes["img"] = "application/octet-stream";
    mimeTypes["msi"] = "application/octet-stream";
    mimeTypes["msp"] = "application/octet-stream";
    mimeTypes["msm"] = "application/octet-stream";
    mimeTypes["mp3"] = "audio/mpeg";
    mimeTypes["ra"]  = "audio/x-realaudio";
    mimeTypes["mpeg"] = "video/mpeg";
    mimeTypes["mpg"] = "video/mpeg";
    mimeTypes["mov"] = "video/quicktime";
    mimeTypes["flv"] = "video/x-flv";
    mimeTypes["avi"] = "video/x-msvideo";
    mimeTypes["wmv"] = "video/x-ms-wmv";
    mimeTypes["asx"] = "video/x-ms-asf";
    mimeTypes["asf"] = "video/x-ms-asf";
    mimeTypes["mng"] = "video/x-mng";
    mimeTypes["data"] = "application/x-www-form-urlencoded";
    mimeTypes["mp4"] = "video/mp4";
    mimeTypes["xml"] = "application/xml";
    
    return mimeTypes;
}

std::map<std::string, std::string> swapMap(std::map<std::string, std::string> m) {
    std::map<std::string, std::string>::iterator it = m.begin();
    for(; it != m.end(); it++)
        m[it->second] = it->first;
    return m;
}

std::vector<std::string> splite(std::string str, std::string delim) {
    std::vector<std::string> splited_elements;
    
    for(size_t i = 0; i < str.size(); i++) {
        std::string s;
        while (str[i] && delim.find(str[i]) != std::string::npos)
            i++;
        while (str[i] && delim.find(str[i]) == std::string::npos)
            s += str[i++];
        if(!s.empty())
            splited_elements.push_back(s);
    }
    return splited_elements;
}

std::string readfile(std::ifstream & file) {
    std::string content;
    std::string line;

    while (getline(file, line))
        content += line + '\n';
    file.close();
    return content;
}

bool    check_extention(std::string file_name) {
    std::string extention = file_name.substr(file_name.size() - 5);
    return extention != ".conf" ? false : true;
}

std::string check_file_errors(std::string file_name) {
    std::ifstream file;

    if(!check_extention(file_name))
        throw "Invalid extention";
    file.open(file_name.c_str());
    if(!file)
        throw strerror(errno);
    return readfile(file);
}

bool    is_number(std::string str) {
    for(size_t i = 0; i < str.size(); i++)
        if(!isdigit(str[i]))
            return false;
    return true;
}

bool    has_duplicate(std::vector<std::string> v) {
    std::map<std::string, bool> m;
    std::vector<std::string>::iterator it = v.begin();

    for(; it != v.end(); it++)
        m[*it] = false;

    it = v.begin();
    for(; it != v.end(); it++) {
        if(m[*it] == true)
            return true;
        m[*it] = true;
    }

    return false;
}

bool is_all_number(std::vector<std::string> v) {
    std::vector<std::string>::iterator it = v.begin();
    for(; it != v.end(); it++)
        if(!is_number(*it))
            return false;
    
    return true;
}

bool is_all_string(std::vector<std::string> v) {
    std::vector<std::string>::iterator it = v.begin();
    for(; it != v.end(); it++)
        if(is_number(*it))
            return false;
    
    return true;
}

Data::Data(servers_map servers) : servers(servers) { }

servers_map Data::getServers() {
    return servers;
}

std::map<int, Client*> &Data::getClientMap()
{
    return clientMap;
}

server * Data::findServer(std::pair<std::string, std::string> host_port, std::string server_name) {
    
    if(servers.find(host_port) == servers.end())
        throw "Server Not Found At The config File";

    std::vector<server*>::iterator it2 = servers[host_port].begin();
    for(; it2 != servers[host_port].end(); it2++)
        if(find((*it2)->directives["server_name"].begin(),  (*it2)->directives["server_name"].end(), server_name) != (*it2)->directives["server_name"].end())
            return *it2;

    return servers[host_port][0];
}

void Data::fillIpPorts(int key, const std::string& value1, const std::string& value2) {
    ipPorts[key] = std::make_pair(value1, value2);
}

std::map<int, std::pair<std::string, std::string> > Data::getIpPorts() {
    return ipPorts;
}


void deleteServer(server * s) {
    std::map<std::string, location*>::iterator it = s->locations.begin();
    for(; it != s->locations.end(); it++)
        delete it->second;
    delete s;
}

void deleteSeversVector(std::vector<server*> servers) {
    std::vector<server*>::iterator it2 = servers.begin();
    for(; it2 != servers.end(); it2++)
        deleteServer(*it2);
}

void deleteServersMap(servers_map servers) {
    servers_map::iterator it = servers.begin();
    for(; it != servers.end(); it++)
        deleteSeversVector(it->second);
}

std::string intToHex(int value) {
    std::stringstream stream;
    stream << std::hex << value;
    return stream.str();
}

bool isPathExist(std::string path) {
    std::ifstream file;
    file.open(path.c_str());
    if(!file)
        return false;
    file.close();
    return true;
}

bool comparePath(std::string s1, std::string s2) {
    std::vector<std::string> v1 = splite(s1, "/");
    std::vector<std::string> v2 = splite(s2, "/");

    if(v1.size() > v2.size())
        return false;

    for(size_t i = 0; i < v1.size(); i++) {
        if(v1[i] != v2[i])
            return false;
    }
    return true;
}

bool hasLocation(std::string s, std::map<std::string, location *> locations, std::string & path) {

    std::map<std::string, location *>::reverse_iterator it = locations.rbegin();
    for(; it != locations.rend(); it++) {
        if(comparePath(it->first, s)) {
            path = it->first;
            return true;
        }
    }
    return false;
}

void    getAbsolutePath(vector_map & directives, std::string url, std::string path) {
    if(directives.find("root") != directives.end()) {
        std::string root = directives["root"][0];
        directives["root"].clear();
        directives["root"].push_back(root + url);
    } else if(directives.find("alias") != directives.end()) {
        std::string s;
        url.size() > path.size() ? s = url.substr(path.size()) : s = "/";
        std::string root = directives["alias"][0];
        directives["root"].clear();
        root[root.size() - 1] == '/' ? directives["root"].push_back(root + s) : directives["root"].push_back(root + "/" + s);
    }
}

std::string getFileFromPath(std::string path) {
    int i = path.rfind("/");
    return path.substr(i);
}

void    saveEnvirements(Client * client) {
    std::map<std::string, std::string> envs;
    
    envs["REQUEST_URI"] = client->request["url"];

    client->request.find("query") !=  client->request.end() ? envs["QUERY_STRING"] = client->request["query"]  : envs["QUERY_STRING"] = "";

    envs["REQUEST_METHOD"] = client->request["method"];
    envs["SCRIPT_FILENAME"] = client->c_location->directives["root"][0];
    envs["SCRIPT_NAME"] = getFileFromPath(client->c_location->directives["root"][0]);
    envs["SERVER_ADDR"] = client->c_location->ipPort.first;
    envs["SERVER_PORT"] = client->c_location->ipPort.second;
    envs["SERVER_NAME"] = client->request["Host"];
    envs["SERVER_SOFTWARE"] = "webserver";
    envs["GATEWAY_INTERFACE"] = "CGI/1.1";
    envs["REDIRECT_STATUS"] = "200";
    envs["CACHE_CONTROL"] = "no-cache";
    envs["SERVER_PROTOCOL"] = client->request["version"];
    envs["CONTENT_TYPE"] = "text/html";
    !client->cookies.empty() ? envs["HTTP_COOKIE"] = client->cookies[0]  : envs["HTTP_COOKIE"] = "";
    if(client->request["method"] == "POST") {
        client->request.find("Content-Type") !=  client->request.end() ? envs["CONTENT_TYPE"] = client->request["Content-Type"]  : envs["CONTENT_TYPE"] = "";
        client->request.find("Content-Length") !=  client->request.end() ? envs["CONTENT_LENGTH"] = client->request["Content-Length"]  : envs["CONTENT_LENGTH"] = "";
    }
    client->env_variables = envs;
}

char    **mapToEnv(std::map<std::string, std::string> envs) {
    std::map<std::string, std::string>::iterator it = envs.begin();
    char **env = new char*[envs.size() + 1];
    int i = 0;
    for(; it != envs.end(); it++) {
        std::string tmp = it->first + "=" + it->second;
        env[i] = strdup(tmp.c_str());
        i++;
    }
    env[envs.size()] = NULL;
    return env;
}

bool isDirectory(std::string path) {
    struct stat fileInfo;
    if (stat(path.c_str(), &fileInfo) != 0)
        return false;
    return S_ISDIR(fileInfo.st_mode);
}


bool checkForBoundary(Client *client) {
    std::string data  = client->request["Content-Type"];
    size_t pos = data.find("multipart/form-data");
    if (pos != std::string::npos) {
        pos = data.find("boundary=", pos);
        if (pos != std::string::npos) {
            size_t boundary_start = pos;
            size_t boundary_end = data.size();
            std::string boundary = data.substr(boundary_start, boundary_end - boundary_start);
            std::size_t pos = boundary.find("=");
            if (pos != std::string::npos) {
                std::string key = boundary.substr(0, pos);
                std::string value = boundary.substr(pos + 1);
                client->request[key] = value;
                if (value.empty())
                {
                    client->Respond = true;
                    client->res.code = 400;
                }
                return true;
            }
        }
    }
    return false;
}

void ckeckBodyNotEnogh(Client &currentClient)
{
    if (currentClient.contentLength >  strToDouble(currentClient.request["Content-Length"]))
    {
        currentClient.Respond = true;
        currentClient.res.code = 400;
        if(currentClient.objFile.is_open())
            currentClient.objFile.close();
        std::remove(currentClient.postFile.c_str());
    }
}

std::string ToString(int number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}