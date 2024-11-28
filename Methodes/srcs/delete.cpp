#include "../includes/delete.hpp"

void deleteElement(std::string file, Client &currentClient)
{
    if (!currentClient.Respond)
    {
        DIR* dir = opendir(file.c_str());
        if (dir)
        {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL)
            {
                if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                    continue;
                std::string entryPath = file + "/" + entry->d_name;

                if (entry->d_type == DT_DIR)
                    deleteElement(entryPath, currentClient);
                else if (entry->d_type == DT_REG)
                {
                    if (!access(entryPath.c_str(), W_OK))
                        std::remove(entryPath.c_str());
                }
            }
            closedir(dir);
            if (!access(file.c_str(), W_OK))
                std::remove(file.c_str());
        }
        else
        {
            if (!access(file.c_str(), W_OK))
                std::remove(file.c_str());
        }
    }
}

void deleteRequest(Client &currentClient)
{
    std::string file = currentClient.c_location->directives["root"][0];
    if (file == currentClient.root + "/" || file == currentClient.root || access(file.c_str(), W_OK))
    {
        currentClient.Respond = true;
        currentClient.res.code = 403;
    }
    else{
        deleteElement(file, currentClient);
        if (!access(file.c_str(), W_OK))
        {
            currentClient.Respond = true;
            currentClient.res.code = 403;
        }
        else
        {
            currentClient.Respond = true;
            currentClient.res.code = 204;
        }
        
    }
}

