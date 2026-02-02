#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "wildcard_list.h"
#include "string_list.h"
#include "wildcard.h"

struct wildcardListCountSortContainer
{
    struct wildcardListNode* wildcardList;
    int weight;
    struct wildcardListCountSortContainer* next;
};

struct wildcardListNode* wildcardList_init()
{
    return (struct wildcardListNode*)NULL;
}

void wildcardList_add(struct wildcardListNode** wildcardListPtr, struct wildcard* contentToAdd)
{
    struct wildcardListNode* currentNode = *wildcardListPtr;

    if (currentNode == (struct wildcardListNode*)NULL)
    {
        currentNode = malloc(sizeof(struct wildcardListNode));

        assert(currentNode != (struct wildcardListNode*)NULL);

        currentNode->nodeContent = contentToAdd;
        currentNode->nextNode = (struct wildcardListNode*)NULL;

        *wildcardListPtr = currentNode;
    }
    else
    {
        while (currentNode->nextNode != (struct wildcardListNode*)NULL)
        {
            currentNode = currentNode->nextNode;
        }

        currentNode->nextNode = malloc(sizeof(struct wildcardListNode));

        assert(currentNode->nextNode != (struct wildcardListNode*)NULL);

        currentNode->nextNode->nodeContent = contentToAdd;
        currentNode->nextNode->nextNode = (struct wildcardListNode*)NULL;
    }
}

void wildcardList_free(struct wildcardListNode** wildcardListPtr)
{
    if ((*wildcardListPtr) != (struct wildcardListNode*)NULL)
    {
        wildcardList_free(&((*wildcardListPtr)->nextNode));
    }

    free(*wildcardListPtr);
    *wildcardListPtr = (struct wildcardListNode*)NULL;
}

bool wildcardList_hasNext(struct wildcardListNode* wildcardList)
{
    return (wildcardList != (struct wildcardListNode*)NULL);
}

struct wildcard* wildcardList_next(struct wildcardListNode** wildcardListPtr)
{
    struct wildcard* result = (struct wildcard*)NULL;
    struct wildcardListNode* list = *wildcardListPtr;

    if (list != (struct wildcardListNode*)NULL)
    {
        result = list->nodeContent;
        *wildcardListPtr = list->nextNode;
    }

    return result;
}

size_t wildcardList_length(struct wildcardListNode* wildcardList)
{
    struct wildcardListNode* currentNode = wildcardList;
    size_t i = 0;

    while (currentNode != (struct wildcardListNode*)NULL)
    {
        i++;
        currentNode = currentNode->nextNode;
    }

    return i;
}

struct wildcard* wildcardList_at(struct wildcardListNode* wildcardList, int i)
{
    struct wildcardListNode* currentNode = wildcardList;
    int currentNodeIndex = 0;
    struct wildcard* result = NULL;

    while ((wildcardList_hasNext(currentNode) == true) && (currentNodeIndex < i))
    {
        wildcardList_next(&currentNode);

        ++currentNodeIndex;
    }

    if (currentNode != NULL)
    {
        result = currentNode->nodeContent;
    }

    return result;
}

void wildcardListCountSortContainer_free(struct wildcardListCountSortContainer** containerToFree)
{
    if ((*containerToFree) != (struct wildcardListCountSortContainer*)NULL)
    {
        wildcardListCountSortContainer_free(&((*containerToFree)->next));
    }

    free(*containerToFree);
    *containerToFree = (struct wildcardListCountSortContainer*)NULL;
}

void wildcardList_sortInPlaceByNumberOfStaticWords(struct wildcardListNode** wildcardListPtr)
{
    struct wildcardListNode* wildcardList = *wildcardListPtr;
    struct wildcardListCountSortContainer* countSortContainer = NULL;
    struct wildcardListCountSortContainer* previousCountSortContainer = NULL;
    struct wildcard* currentWildcard;
    int currentWildcardNumberOfStaticWords;
    int maximumDepthInCountSortContainer = 0;
    struct wildcardListCountSortContainer* currentCountSortContainer;
    struct wildcardListNode* result = NULL;
    struct wildcardListNode* currentWildcardListInCurrentCountSortContainer;
    struct wildcardListNode* currentWildcardListInResult;

    // Building count sort container
    while (wildcardList_hasNext(wildcardList) == true)
    {
        currentWildcard = wildcardList_next(&wildcardList);
        currentWildcardNumberOfStaticWords = stringList_length(currentWildcard->staticWords);

        if (currentWildcard->begin == STAR)
        {
            currentWildcardNumberOfStaticWords++;
        }

        while (maximumDepthInCountSortContainer <= currentWildcardNumberOfStaticWords)
        {
            countSortContainer = malloc(sizeof(struct wildcardListCountSortContainer));

            countSortContainer->wildcardList = wildcardList_init();
            countSortContainer->weight = maximumDepthInCountSortContainer;
            countSortContainer->next = previousCountSortContainer;

            ++maximumDepthInCountSortContainer;

            previousCountSortContainer = countSortContainer;
        }

        currentCountSortContainer = countSortContainer;

        while ((currentCountSortContainer != NULL) && (currentCountSortContainer->weight != currentWildcardNumberOfStaticWords))
        {
            currentCountSortContainer = currentCountSortContainer->next;
        }

        wildcardList_add(&currentCountSortContainer->wildcardList, currentWildcard);
    }

    // Doing count sort
    result = countSortContainer->wildcardList; // We take the most weighted list for the beginning of the result
    currentWildcardListInResult = result;

    if (countSortContainer->next != NULL)
    {
        currentCountSortContainer = countSortContainer->next;

        while (currentCountSortContainer != NULL)
        {
            currentWildcardListInCurrentCountSortContainer = currentCountSortContainer->wildcardList;

            if (currentWildcardListInCurrentCountSortContainer != NULL)
            {
                while (currentWildcardListInResult->nextNode != NULL)
                {
                    currentWildcardListInResult = currentWildcardListInResult->nextNode;
                }

                currentWildcardListInResult->nextNode = currentWildcardListInCurrentCountSortContainer;
            }

            currentCountSortContainer = currentCountSortContainer->next;
        }
    }

    // Cleaning and returning
    wildcardListCountSortContainer_free(&countSortContainer);
    wildcardList_free(wildcardListPtr);
    *wildcardListPtr = result;
}

struct wildcard* wildcardList_findWildcardByWildcardTag(struct wildcardListNode* wildcardList, char* tag)
{
    struct wildcard* result = (struct wildcard*)NULL;
    bool found = false;
    struct wildcard* currentWildcard;
    struct wildcardListNode* wildcardListIterator = wildcardList;

    while ((wildcardList_hasNext(wildcardListIterator) == true) && (found == false))
    {
        currentWildcard = wildcardList_next(&wildcardListIterator);

        if (strcmp(currentWildcard->tag, tag) == 0)
        {
            found = true;
            result = currentWildcard;
        }
    }

    return result;
}