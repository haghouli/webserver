#include "../File_parser/includes/response.hpp"

void freeArray(char ** arr) {
    int i = 0;
    while(arr[i])
        free(arr[i++]);
}

bool    checkTimeOut(clock_t start, double maxTime) {
    clock_t diff = clock() - start;
    if(maxTime < static_cast<double>(diff / CLOCKS_PER_SEC))
        return true;
    return false;
}

void    chekCgiReturn(Client * client, int ret) {
    if(checkTimeOut(client->cgiTimeStart, strToDouble(client->c_location->directives["request_cgi_timeout"][0]))) {
        kill(client->cgiProcessId, SIGKILL);
        waitpid(client->cgiProcessId, NULL, 0);
        client->hasCgi = false;
        client->res.code = 504;
        client->Respond = true;
        client->isCgiInProcess = false;
        if(client->request["method"] == "POST")
        {
            if (client->objFile.is_open())
                client->objFile.close();
            std::remove(client->postFile.c_str());
        }
    } else if(waitpid(client->cgiProcessId, &ret, WNOHANG) != 0) {
        client->Respond = true;
        if(WEXITSTATUS(ret) != 0) {
            client->hasCgi = false;
            client->res.code = 500;
            if(client->request["method"] == "POST")
            {
                if (client->objFile.is_open())
                    client->objFile.close();
                std::remove(client->postFile.c_str());
            }
        } else {
            client->res.code = 200;
            client->c_location->locationFile.open(client->cgiOutputFile.c_str(), std::ios::in);
            if(client->request["method"] == "POST")
            {
                if (client->objFile.is_open())
                    client->objFile.close();
                std::remove(client->postFile.c_str());
            }
        }
    } else {
        client->Respond = false;
        client->isCgiInProcess = true;
    }
}

void    execCGI(Client *client, char *args[]) {
    std::string contentType = client->request["Content-Type"];
    std::string extentionPost   = client->c_location->mime_types[contentType];
    FILE * f1 = NULL, *f2 = NULL;
    if(client->request["method"] == "POST") {
        client->env_variables["CONTENT_LENGTH"]  = ToString(client->contentLength);
        client->env_variables["DOCUMENT_ROOT"]   = client->c_location->path;
        client->env_variables["EXTENTION"]       = extentionPost;
        if((f1 = freopen(client->postFile.c_str(), "r", stdin)) == NULL) {
            if (client->objFile.is_open())
                client->objFile.close();
            std::remove(client->postFile.c_str());
            client->res.code = 500;
            client->Respond = true;
            return ;
        }
    }
    if((f2 = freopen(client->cgiOutputFile.c_str(), "w", stdout)) == NULL) {
        if(client->request["method"] == "POST")
        {
            if (client->objFile.is_open())
                client->objFile.close();
            std::remove(client->postFile.c_str());
        }
        client->res.code = 500;
        client->Respond = true;
        return ;
    }
    char **env = mapToEnv(client->env_variables);
    execve(args[0], args, env);
    freeArray(env);
    if(f1)
        fclose(f1);
    if(f2)
        fclose(f2);
    delete [] env;
    exit(1);
}

void   runCGI(std::string extention, Client *client) {
    int ret                         = 0;
    if(!client->isCgiInProcess) {
        char                        *args[3];
        std::string scriptRoot          = getFileRoot(client->c_location->directives["root"][0]);
        client->cgiOutputFile           = scriptRoot + generateRandomFile("cgiOutput", scriptRoot);

        args[0] = const_cast<char *>(client->c_location->cgis[extention].c_str());
        args[1] = const_cast<char *>(client->c_location->directives["root"][0].c_str());
        args[2] = NULL;

        client->cgiTimeStart = clock();
        int id = fork();
        client->cgiProcessId = id;
        if(id == -1) {
            client->hasCgi = false;
            client->res.code = 500;
            client->Respond = true;
            return ;
        } if(!id)
            execCGI(client, args);
        waitpid(id, &ret, WNOHANG);
        if(WEXITSTATUS(ret) != 0) {
            client->hasCgi = false;
            client->res.code = 500;
            client->Respond = true;
            return;
        }
    }
    chekCgiReturn(client, ret);
}
