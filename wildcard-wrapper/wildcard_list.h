#ifndef WILDCARD_LIST_H
#define WILDCARD_LIST_H

#include <stddef.h>
#include <stdbool.h>

#include "wildcard.h"

struct wildcardListNode
{
    struct wildcard* nodeContent;
    struct wildcardListNode* nextNode;
};

struct wildcardListNode* wildcardList_init();
void wildcardList_add(struct wildcardListNode** wildcardListPtr, struct wildcard* contentToAdd);
void wildcardList_free(struct wildcardListNode** wildcardListPtr);
bool wildcardList_hasNext(struct wildcardListNode* wildcardList);
struct wildcard* wildcardList_next(struct wildcardListNode** wildcardListPtr);
size_t wildcardList_length(struct wildcardListNode* wildcardList);
struct wildcard* wildcardList_at(struct wildcardListNode* wildcardList, int i);
void wildcardList_sortInPlaceByNumberOfStaticWords(struct wildcardListNode** wildcardListPtr);
struct wildcard* wildcardList_findWildcardByWildcardTag(struct wildcardListNode* wildcardList, char* tag);

#endif