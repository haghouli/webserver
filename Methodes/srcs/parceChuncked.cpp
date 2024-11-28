#include "../includes/Post.hpp"

void getChunck(Client &currentClient)
{
    std::size_t pos;
    std::string chunkSizeHexa;
    pos =  currentClient.body.find("\r\n");
    chunkSizeHexa = currentClient.body.substr(0, pos);
    currentClient.currentChunkSize = hexaToDic(chunkSizeHexa);
    currentClient.bodySize -= (pos + 2);
    currentClient.contentLength -= (pos + 2);
    currentClient.body = currentClient.body.substr(pos  + 2, currentClient.bodySize);
}

void postCurrentBody(Client &currentClient)
{
    currentClient.objFile.write(currentClient.body.data(), currentClient.bodySize);
    if(currentClient.objFile.fail())
    {
        currentClient.Respond = true;
        currentClient.res.code = 500;
    }
    currentClient.objFile.flush();
    currentClient.currentChunkSize -= currentClient.bodySize; 
    currentClient.body.clear();
    currentClient.bodySize = 0;
}

void postChunckAndSubBody(Client &currentClient)
{
    currentClient.bodySize  -= (currentClient.currentChunkSize);
    std::string currentChunk = currentClient.body.substr(0, currentClient.currentChunkSize);
    currentClient.objFile.write(currentChunk.data(), currentClient.currentChunkSize);
    if(currentClient.objFile.fail())
    {
        currentClient.Respond = true;
        currentClient.res.code = 500;
    }
    currentClient.objFile.flush();
    currentClient.body =  currentClient.body.substr(currentClient.currentChunkSize , currentClient.bodySize);
    currentClient.currentChunkSize = 0;
}

void CheckEndOfChunkedEncoding(Client &currentClient)
{
    removeCRLF(currentClient);
    if( currentClient.body == "0\r\n\r\n")
    {
        if (!checkLength(currentClient, currentClient.contentLength))
            return;
        currentClient.contentLength -= 5;
        currentClient.dataUploaded = true;
        if (currentClient.objFile.is_open())
            currentClient.objFile.close();
    }
}

void postToFileChuncked(Client &currentClient)
{
    if(!currentClient.Respond && getPath(currentClient))
    {
        std::string contentType = currentClient.request["Content-Type"];
        if (getFile(currentClient, contentType))
        {
            checkPermitions(currentClient);
            if (!checkLength(currentClient, currentClient.contentLength))
                return;
            if (!currentClient.currentChunkSize)
            {
                CheckEndOfChunkedEncoding(currentClient);
                getChunck(currentClient);
            }
            if (currentClient.bodySize <= currentClient.currentChunkSize)
                postCurrentBody(currentClient);
            else if (currentClient.bodySize > currentClient.currentChunkSize)
            {
                postChunckAndSubBody(currentClient);
                CheckEndOfChunkedEncoding(currentClient);
            }
        }
    }
}
