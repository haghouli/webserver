#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stdlib.h>

#include "../../File_parser/includes/utiles.hpp"
#include "../../Multiplexing/includes/Multi.hpp"
#include "../../cgi/cgi.hpp"

		/*****************UTILES***********************	*/

std::size_t     getFile(Client &currentClient, std::string);
bool  			getPath(Client &currentClient);
double          bytesToMegabytes(size_t bytes);
void 			removeCRLF(Client &currentClient);
bool            checkLength(Client &currrentClient, double contentSize);
size_t          hexaToDic(std::string &hexString);
std::string     generateFileName(Client &currentClient, std::string);
void 			postToFileContentLength(std::string &chunk,  Client &currentClient);
void 			postRequest(Client &currentClient);
void 			checkPermitions(Client &currentClient);

		/************************************************/


		/*******************CHUNKED**********************/

void 			getChunck(Client &currentClient);
void 			postCurrentBody(Client &currentClient);
void 			postChunckAndSubBody(Client &currentClient);
void 			CheckEndOfChunkedEncoding(Client &currentClient);
void 			postToFileChuncked(Client &currentClient);
void 			postToFileBoundry(Client &currentClient, std::string &chunk);
bool 			checkForBoundary(Client *client);
		/************************************************/


/***********************Boundary*************************/
std::string 	getValue(std::size_t start, std::string str);
void  			parceHeader(Client &currentClient, std::string header);
bool 			parcePart(Client &currentClient, std::string &chunk);