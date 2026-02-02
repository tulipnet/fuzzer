#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "filesystem.h"
#include "config.h"
#include "string_list.h"
#include "utils.h"

struct stringListNode* getDirectoryContent(char* directoryPath)
{
    struct stringListNode* result = stringList_init();
    DIR* directoryFD;
    struct dirent* directoryEntry;
    char* currentFilePath;
    size_t currentFilePathLength;

    directoryFD = opendir(directoryPath);

    if (directoryFD != NULL)
    {
        directoryEntry = readdir(directoryFD);

        while (directoryEntry != NULL)
        {
            if ((strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0))
            {
                currentFilePathLength = strlen(directoryPath) + strlen(directoryEntry->d_name) + 5;
                currentFilePath = malloc(sizeof(char) * currentFilePathLength);

                strncpy(currentFilePath, directoryPath, currentFilePathLength);
                strConcat(currentFilePath, "/", currentFilePathLength);
                strConcat(currentFilePath, directoryEntry->d_name, currentFilePathLength);

                stringList_add(&result, currentFilePath);
            }

            directoryEntry = readdir(directoryFD);
        }

        closedir(directoryFD);
    }

    return result;
}