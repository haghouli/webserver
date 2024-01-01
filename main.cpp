#include "./File_parser/includes/utiles.hpp"
#include "./Multiplexing/Multi.hpp"

int main(int ac, char **av) {
    try
    {
        Data * data = getConfig(ac, av);
        startTheServer(data);
    }
    catch (const char *msg) {
        std::cout << msg << std::endl;
    }
    catch (std::string msg) {
        std::cout << msg << std::endl;
    }
}

/*

    Define a directory or a file from where the file should be searched (for example,
    if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
    /tmp/www/pouic/toto/pouet).

    /kapouet/pouic/toto/pouet

    location /kapouet {
        root    /tmp/www;
    }

    /tmp/www/pouic/toto/pouet

*/
