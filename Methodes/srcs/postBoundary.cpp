#include "../includes/Post.hpp"

void startFile(Client &currentClient, std::string &chunk)
{
    if (parcePart(currentClient, chunk))
    {
        if (getFile(currentClient, currentClient.boundaryType))
            currentClient.boundaryStarts = true;
    }
}

void postsubChunk(Client &currentClient, std::string &chunk, size_t size)
{
    checkPermitions(currentClient);
    std::string postChunck;
    postChunck = chunk.substr(0, size);
    chunk.erase(0, size);
    currentClient.bodySize -= size;
    currentClient.objFile.write(postChunck.data(), size);
    if(currentClient.objFile.fail())
    {
        currentClient.Respond = true;
        currentClient.res.code = 500;
    }
    currentClient.objFile.flush();
    if ( currentClient.objFile.is_open())
        currentClient.objFile.close();
}

void postChunk(Client &currentClient, std::string &chunk)
{

    checkPermitions(currentClient);
    currentClient.objFile.write(chunk.data(), currentClient.bodySize);
    if(currentClient.objFile.fail())
    {
        currentClient.Respond = true;
        currentClient.res.code = 500;
    }
    currentClient.bodySize = 0;
    chunk.clear();
    currentClient.objFile.flush();
}

void postToFileBoundry(Client &currentClient, std::string &chunk)
{
    std::string boundary = "--" + currentClient.request["boundary"];
    std::size_t pos = chunk.find(boundary);
    if (!checkLength(currentClient, currentClient.contentLength))
        return;
    if (!currentClient.Respond && getPath(currentClient))
    {
        if(pos == 0 && chunk.find(boundary + "--") != 0 && !currentClient.boundaryStarts)
            startFile(currentClient, chunk);
        else if (currentClient.boundaryStarts)
        {
            if (pos == std::string::npos)
                postChunk(currentClient, chunk);
            else if (pos != std::string::npos)
            {
                postsubChunk(currentClient, chunk, pos);
                currentClient.boundaryStarts = false;
            }
        }
        if (!chunk.empty() && chunk.find(boundary + "--") == 0)
        {
            if (currentClient.objFile.is_open())
                currentClient.objFile.close();
            currentClient.dataUploaded = true;
        }
    }
}
