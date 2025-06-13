#ifndef DIRHANDLE_H_INCLUDED
#define DIRHANDLE_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>      // For _mkdir
    #define SYSTEM 1         // Windows
#elif __linux__
    #include <sys/types.h>
    #define SYSTEM 2         // Linux
#else
    #define SYSTEM 0         // Unknown system
#endif

bool createFolder(const char* dirName);
bool folderExists(const char* path);

#endif // DIRHANDLE_H_INCLUDED
