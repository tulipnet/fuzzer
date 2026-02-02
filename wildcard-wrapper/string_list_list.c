#include "string_list_list.h"

struct stringListListNode* stringListList_init()
{
    return (struct stringListListNode*)NULL;
}

void stringListList_add(struct stringListListNode** stringListListPtr, struct stringListNode* stringListToAdd)
{
    struct stringListListNode* currentNode = *stringListListPtr;

    if (currentNode == (struct stringListListNode*)NULL)
    {
        currentNode = malloc(sizeof(struct stringListListNode));

        assert(currentNode != (struct stringListListNode*)NULL);

        currentNode->nodeContent = stringListToAdd;
        currentNode->nextNode = (struct stringListListNode*)NULL;

        *stringListListPtr = currentNode;
    }
    else
    {
        while (currentNode->nextNode != (struct stringListListNode*)NULL)
        {
            currentNode = currentNode->nextNode;
        }

        currentNode->nextNode = malloc(sizeof(struct stringListListNode));

        assert(currentNode->nextNode != (struct stringListListNode*)NULL);

        currentNode->nextNode->nodeContent = stringListToAdd;
        currentNode->nextNode->nextNode = (struct stringListListNode*)NULL;
    }
}

void stringListList_free(struct stringListListNode** stringListListPtr)
{
    if ((*stringListListPtr) != (struct stringListListNode*)NULL)
    {
        stringListList_free(&((*stringListListPtr)->nextNode));
    }

    free(*stringListListPtr);
    *stringListListPtr = (struct stringListListNode*)NULL;
}

bool stringListList_hasNext(struct stringListListNode* stringListList)
{
    return (stringListList != (struct stringListListNode*)NULL);
}

struct stringListNode* stringListList_next(struct stringListListNode** stringListListPtr)
{
    struct stringListNode* result = (struct stringListNode*)NULL;
    struct stringListListNode* list = *stringListListPtr;

    if (list != (struct stringListListNode*)NULL)
    {
        result = list->nodeContent;
        *stringListListPtr = list->nextNode;
    }

    return result;
}

size_t stringListList_length(struct stringListListNode* stringList)
{
    struct stringListListNode* currentNode = stringList;
    size_t i = 0;

    while (currentNode != (struct stringListListNode*)NULL)
    {
        i++;
        currentNode = currentNode->nextNode;
    }

    return i;
}