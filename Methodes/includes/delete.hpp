#pragma once

#include "../../File_parser/includes/response.hpp"
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <filesystem>
#include <cstdio>
#include <dirent.h>
#include "Post.hpp"

void deleteElement(std::string file, Client &currentClient);
void deleteRequest(Client &currentClient);