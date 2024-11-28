#include "../includes/Post.hpp"

size_t x = 0;
void postToFileContentLength(std::string &chunk, Client &currentClient)
{
    ckeckBodyNotEnogh(currentClient);
    if (getPath(currentClient))
    {
        std::string contentType = currentClient.request["Content-Type"];
        size_t bodySize = strToDouble(currentClient.request["Content-Length"]);

        if (!currentClient.Respond && !checkLength(currentClient, bodySize) )
            return;
        if (getFile(currentClient, contentType))
        {
            checkPermitions(currentClient);
            currentClient.objFile.write(chunk.data(), currentClient.bodySize);
            if(currentClient.objFile.fail())
            {
                currentClient.Respond = true;
                currentClient.res.code = 500;
            }
            currentClient.bodySize = 0;
            currentClient.objFile.flush();
            chunk.clear();
            if (currentClient.contentLength ==  strToDouble(currentClient.request["Content-Length"]))
            {
                if(currentClient.objFile.is_open())
                    currentClient.objFile.close();
                currentClient.dataUploaded = true;
            }
        }  
    }
}

void postRunCgi(Client * client, std::string extention) {
    client->hasCgi = true;
    if(client->isFirstTime)
        runCGI(extention, client);
    if(client->Respond) {
        if(client->res.code < 400) {
            char * fileContent = readChuncks(client, client->c_location->locationFile);
            if(client->hasCgi) {
                fillGetResponse(client, client->res.code, "text/html", fileContent, "", client->c_location->locationFile.gcount());
                delete [] fileContent;
            }

        } else
            client->hasCgi = false;
    }
}

void postRequest(Client &currentClient)
{
    std::string extention = getExtension(currentClient.c_location->directives["root"][0]);
    if  (!currentClient.dataUploaded && !currentClient.Respond)
    {
        if (checkForBoundary(&currentClient))
        {
            postToFileBoundry(currentClient, currentClient.body);
        }
        else if (currentClient.request["Transfer-Encoding"] == "chunked")
        {
            postToFileChuncked(currentClient);
        }
        else
        {
            postToFileContentLength(currentClient.body, currentClient);
        }
    }
    if (currentClient.dataUploaded)
    {
        if (hasCgi(extention, currentClient.c_location->cgis))
            postRunCgi(&currentClient, extention);
        else
        {
            std::string msg = "<html>Content uploaded!</html>";
            fillGetResponse(&currentClient, 201, "text/html", msg.c_str(), "", msg.size());
            currentClient.Respond = true;
        }
    }
    
}

