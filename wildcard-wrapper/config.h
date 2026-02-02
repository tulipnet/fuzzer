#ifndef CONFIG_H
#define CONFIG_H

#define MAX_FILE_WRAPPER                (1 * 1024 * 1024L) + 100
#define MAX_STRING_SIZE                 MAX_FILE_WRAPPER
#define WILDCARD_FILE_READ_BUFFER_SIZE  MAX_FILE_WRAPPER
#define ENV_WILDCARD_PATH               "WILDCARD_PATH"
#define READ_BUFFER_SIZE                MAX_FILE_WRAPPER
#define ENV_SHOW_WILDCARDS              "SHOW_WILDCARDS"
#define DUMMY_CONCRETE_DATA             "aaaaa"

#endif