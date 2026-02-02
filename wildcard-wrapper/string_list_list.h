#ifndef STRING_LIST_LIST_H
#define STRING_LIST_LIST_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "string_list.h"

struct stringListListNode
{
    struct stringListNode* nodeContent;
    struct stringListListNode* nextNode;
};

struct stringListListNode* stringListList_init();
void stringListList_add(struct stringListListNode** stringListListPtr, struct stringListNode* stringListToAdd);
void stringListList_free(struct stringListListNode** stringListListPtr);
bool stringListList_hasNext(struct stringListListNode* stringListList);
struct stringListNode* stringListList_next(struct stringListListNode** stringListListPtr);
size_t stringListList_length(struct stringListListNode* stringList);

#endif