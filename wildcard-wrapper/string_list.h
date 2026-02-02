#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct stringListNode
{
    char* nodeContent;
    struct stringListNode* nextNode;
};

struct stringListNode* stringList_init();
void stringList_add(struct stringListNode** stringListPtr, char* contentToAdd);
void stringList_free(struct stringListNode** stringListPtr);
void stringList_iter(struct stringListNode* stringList, void (*action)(char* str));
void stringList_iteri(struct stringListNode* stringList, void (*action)(char* str, int i));
void stringList_iterd(struct stringListNode* stringList, void (*action)(char* str, void* data), void* data);
bool stringList_hasNext(struct stringListNode* stringList);
char* stringList_next(struct stringListNode** stringListPtr);
void stringList_map_inplace(struct stringListNode* stringList, void (*transformation)(char** str));
void stringList_map_inplace_d(struct stringListNode* stringList, void (*transformation)(char** str, void* data), void* data);
size_t stringList_length(struct stringListNode* stringList);
size_t stringList_allStringsLength(struct stringListNode* stringList);
char* stringList_at(struct stringListNode* stringList, int i);
bool stringList_stringBeginsWithAtLeastOneStringOfThisStringList(char* str, struct stringListNode* stringList);

#endif