#ifndef WRAPPER_INTERNALS_H
#define WRAPPER_INTERNALS_H

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stddef.h>

#include "config.h"
#include "string_list.h"
#include "utils.h"
#include "string_list_list.h"
#include "wildcard_list.h"

char* readWildcard(char* wildcardFileName, ssize_t (*readFunction)(int, void*, size_t));
struct stringListNode* parseExclusions(char* exclusionsString);
struct wildcard* parseWildcard(char* wildcardContent, char* wildcardTag);
size_t fillWildcard(char* readBuffer, size_t readBufferSize, struct wildcard* parsedWildcard, void (*actionOnExclusion)());
void reverseWildcard(char* wildcardedInput, char* outputBuffer, size_t outputBufferSize, struct wildcard* wildcard);
bool isFromThisWildcard(char* input, struct wildcard* wildcard);
struct wildcard* getAssociatedWildcard(char* wildcardedInput, struct wildcardListNode* wildcards);
size_t lengthNeededToFillWildcard(char* input, struct wildcard* wildcard);
bool stringList2WildcardWithStringExclusions(char* outputBuffer, size_t outputBufferSize, struct stringListNode* strings);
size_t getNumberOfWildcardsInWildcard(struct wildcard* wildcard);

#endif