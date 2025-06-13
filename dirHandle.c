#include "dirHandle.h"

bool createFolder(const char* dirName) {
    #if SYSTEM == 1
        if (_mkdir(dirName) == 0)
            return true;
    #elif SYSTEM == 2
        if (mkdir(dirName, 0755) == 0) {
            printf("\nThis program was designed for Windows. Linux could have some problems.");
            return true;
        }
    #else
        printf("\nThis program is not compatible with your operating system.");
    #endif
    return false;
}

bool folderExists(const char* path) {
    struct stat info;

    if (stat(path, &info) != 0)
        return false;

    if (info.st_mode & S_IFDIR)
        return true;

    return false;
}



