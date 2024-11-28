
#ifndef UTILES_HPP
#define UTILES_HPP

#include "./header.hpp"
#include "./response.hpp"

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

typedef std::map<std::pair<std::string, std::string>, std::vector<server*> > servers_map;


class Data {
    private:
        servers_map servers;
        std::map<int, Client*>  clientMap;
        std::map<int, std::pair<std::string, std::string> > ipPorts;
    
    public:
        Data(servers_map servers);
        servers_map                                         getServers();
        std::map<int, Client*>                              &getClientMap();
        server *                                            findServer(std::pair<std::string, std::string> host_port, std::string server_name);
        void                                                fillIpPorts(int key, const std::string& value1, const std::string& value2);
        std::map<int, std::pair<std::string, std::string> > getIpPorts();

        std::map<int, size_t>  readIndexs;
};

template <typename T>
std::string NumberToString(T number) {
    std::ostringstream ss;
    ss << number;
    return ss.str();
}

std::vector<std::string>                    splite(std::string str, std::string delim);
std::string                                 readfile(std::ifstream & file);
std::string                                 check_file_errors(std::string file_name);
bool                                        is_number(std::string str);
bool                                        has_duplicate(std::vector<std::string> v);
bool                                        is_all_number(std::vector<std::string> v);
bool                                        is_all_string(std::vector<std::string> v);

Data *                                      getConfig(int ac, char **av);

void                                        deleteServer(server * s);
void                                        deleteSeversVector(std::vector<server*> servers);
void                                        deleteServersMap(servers_map servers);

void                                        genarateResponse(Client * client);

std::string                                 genarateResponse(int code, std::string msg_code, std::string content, std::string content_type);
char                                        **mapToEnv(std::map<std::string, std::string> envs);
void                                        getAbsolutePath(vector_map & directives, std::string url, std::string path);
void                                        saveEnvirements(Client * client);
bool                                        isPathExist(std::string path);
bool                                        hasLocation(std::string s, std::map<std::string, location *> locations, std::string & path);

std::map<std::string, std::string>          getAllMimeTypes();
void                                        fillErrorPages(Client * client, server * s);

bool                                        isDirectory(std::string path);

std::map<std::string, std::string>          swapMap(std::map<std::string, std::string> m);

bool                                        isSubdirectory(std::string root, std::string subDire);
std::string                                 getClairPath(std::string subDire);

double                                      convetToMb(std::string str);
double                                      convertToSeconds(std::string str);

double                                      strToDouble(std::string str);

std::string                                 generateRandomFile(std::string filePrefix, std::string root);

bool                                        hasCgi(std::string extention, std::map<std::string, std::string> cgisMap);
std::string                                 getExtension(std::string path);

std::string                                 getFileRoot(std::string path);

std::string                                 generateResponse(std::string code);

std::string                                 generateResponse(std::string code, std::string message);
std::string 								ToString(int number);

std::string                                 &trim(std::string& input);

std::string                                 intToHex(int value);

char                                        *readChuncks(Client * client, std::ifstream & file);

void                                        fillGetResponse(Client * client, int code, std::string type,  const char * content, std::string l, int content_size);

std::string                                 getMimeType(std::map<std::string, std::string> mime_types, std::string extention);

bool                                        isDouble(const std::string & s);

bool                                        checkForBoundary(Client *client);

bool                                        checkLengthErr(Client &currentClient);

std::string                                 &strTrim(std::string& input, std::string s);

std::string                                 getRealPath(const char * path);

void                                        ckeckBodyNotEnogh(Client &currentClient);

#endif
