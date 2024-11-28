#include "./File_parser/includes/response.hpp"
#include "./Multiplexing/includes/Multi.hpp"

int main(int ac, char **av) {
    try
    {
        Data * data = getConfig(ac, av);
        servers_map    server = data->getServers();
        startTheServer(data);
        deleteServersMap(server);
        delete data;
    }
    catch (const char *msg) {
        std::cerr << msg << std::endl;
    }
    catch (std::string msg) {
        std::cerr << msg << std::endl;
    }
}
