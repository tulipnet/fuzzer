#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "wrapper_internals.h"
#include "config.h"
#include "string_list.h"
#include "string_list_list.h"
#include "utils.h"
#include "wildcard.h"
#include "wildcard_list.h"

char* readWildcard(char* wildcardFileName, ssize_t (*readFunction)(int, void*, size_t))
{
    char* result = calloc(WILDCARD_FILE_READ_BUFFER_SIZE, sizeof(char));
    int fdWildcardFilePath;

    assert(result != (char*)NULL);

    fdWildcardFilePath = open(wildcardFileName, O_RDONLY);

    readFunction(fdWildcardFilePath, result, WILDCARD_FILE_READ_BUFFER_SIZE * sizeof(char));
    close(fdWildcardFilePath);

    return result;
}

struct stringListNode* parseExclusions(char* exclusionsString)   // exclusionsString : "toto,titi,tutu"
{
    struct stringListNode* result = stringList_init();
    size_t exclusionsStringLength = strlen(exclusionsString) + 1;
    char* parsedExclusionsString = malloc(exclusionsStringLength * sizeof(char));

    assert(parsedExclusionsString != (char*)NULL);

    strcpy(parsedExclusionsString, exclusionsString);

    parsedExclusionsString = strtok(exclusionsString, ",");
    stringList_add(&result, parsedExclusionsString);

    while (parsedExclusionsString != (char*)NULL)
    {
        parsedExclusionsString = strtok((char*)NULL, ",");

        if (parsedExclusionsString != (char*)NULL)
        {
            stringList_add(&result, parsedExclusionsString);
        }
    }

    return result;
}

struct wildcard* parseWildcard(char* wildcardContent, char* wildcardTag)
{
    struct wildcard* result = malloc(sizeof(struct wildcard));
    size_t wildcardContentLength;
    struct stringListNode* currentExclusions;
    char* strtokSavePtr;
    char* strtokSavePtrParsingExclusions;
    size_t tagLength;

    assert(result != (struct wildcard*)NULL);

    result->starExclusions = stringListList_init();
    result->staticWords = stringList_init();

    if (wildcardTag != NULL)
    {
        tagLength = strlen(wildcardTag) + 1;
        result->tag = malloc(tagLength * sizeof(char));

        strncpy(result->tag, wildcardTag, tagLength);
    }
    else
    {
        result->tag = NULL;
    }

    wildcardContentLength = strnlen(wildcardContent, MAX_STRING_SIZE);

    if (wildcardContent[0] == '*')
    {
        result->begin = STAR;

        wildcardContent = strtok_r(wildcardContent, "*", &strtokSavePtr);

        if ((wildcardContentLength > 2) && (wildcardContent[0] == '-') && (wildcardContent[1] == '{'))
        {
            wildcardContent = strtok_r(wildcardContent, "}", &strtokSavePtrParsingExclusions);
            wildcardContent += 2; // Delete "-{"

            currentExclusions = parseExclusions(wildcardContent);
            stringListList_add(&result->starExclusions, currentExclusions);

            wildcardContent = strtok_r((char*)NULL, "}", &strtokSavePtrParsingExclusions);
        }
    }
    else
    {
        result->begin = STATIC_WORD;

        wildcardContent = strtok_r(wildcardContent, "*", &strtokSavePtr);

        stringList_add(&result->staticWords, wildcardContent);

        wildcardContent = strtok_r((char*)NULL, "*", &strtokSavePtr);
    }

    while (wildcardContent != (char*)NULL)
    {
        wildcardContentLength = strlen(wildcardContent);

        if ((wildcardContentLength > 2) && (wildcardContent[0] == '-') && (wildcardContent[1] == '{'))
        {
            wildcardContent = strtok_r(wildcardContent, "}", &strtokSavePtrParsingExclusions);
            wildcardContent += 2; // Delete "-{"

            currentExclusions = parseExclusions(wildcardContent);
            stringListList_add(&result->starExclusions, currentExclusions);

            wildcardContent = strtok_r((char*)NULL, "}", &strtokSavePtrParsingExclusions);
        }

        stringList_add(&result->staticWords, wildcardContent);
        wildcardContent = strtok_r((char*)NULL, "*", &strtokSavePtr);
    }

    return result;
}

size_t fillWildcard(char* readBuffer, size_t readBufferSize, struct wildcard* parsedWildcard, void (*actionOnExclusion)())
{
    static char originallyReadBuffer[MAX_STRING_SIZE * 32];

    struct stringListNode* parsedWildcardStaticWordsIterator = parsedWildcard->staticWords;
    struct stringListListNode* parsedWildcardStarExclusionsIterator = parsedWildcard->starExclusions;
    char* nextLineInOriginallyReadBuffer;
    char* currentWildcardKeyword = stringList_next(&parsedWildcardStaticWordsIterator);
    struct stringListNode* currentStarExclusionIterator = stringListList_next(&parsedWildcardStarExclusionsIterator);
    char* currentStarExclusion;

    memcpy(originallyReadBuffer, readBuffer, min(readBufferSize, MAX_STRING_SIZE * 32) * sizeof(char));
    originallyReadBuffer[readBufferSize] = '\0';

    strReset(readBuffer, readBufferSize);

    if (parsedWildcard->begin == STATIC_WORD)
    {
        strConcatLeft(readBuffer, currentWildcardKeyword, readBufferSize);
        currentWildcardKeyword = stringList_next(&parsedWildcardStaticWordsIterator);
    }

    nextLineInOriginallyReadBuffer = strtok(originallyReadBuffer, "\n");

    if (nextLineInOriginallyReadBuffer != (char*)NULL)
    {
        while (stringList_hasNext(currentStarExclusionIterator) == true)
        {
            currentStarExclusion = stringList_next(&currentStarExclusionIterator);

            if ((actionOnExclusion != NULL) && (strBeginsWith(currentStarExclusion, nextLineInOriginallyReadBuffer) == true))
            {
                actionOnExclusion();
            }
        }

        strConcat(readBuffer, nextLineInOriginallyReadBuffer, readBufferSize);
        currentStarExclusionIterator = stringListList_next(&parsedWildcardStarExclusionsIterator);
    }

    while (currentWildcardKeyword != (char*)NULL)
    {
        nextLineInOriginallyReadBuffer = strtok((char*)NULL, "\n");

        strConcat(readBuffer, currentWildcardKeyword, readBufferSize);
        currentWildcardKeyword = stringList_next(&parsedWildcardStaticWordsIterator);

        if (nextLineInOriginallyReadBuffer != (char*)NULL)
        {
            while (stringList_hasNext(currentStarExclusionIterator) == true)
            {
                currentStarExclusion = stringList_next(&currentStarExclusionIterator);

                if ((actionOnExclusion != NULL) && (strBeginsWith(currentStarExclusion, nextLineInOriginallyReadBuffer) == true))
                {
                    actionOnExclusion();
                }
            }

            strConcat(readBuffer, nextLineInOriginallyReadBuffer, readBufferSize);
            currentStarExclusionIterator = stringListList_next(&parsedWildcardStarExclusionsIterator);
        }
    }

    return strnlen(readBuffer, readBufferSize);
}

void reverseWildcard(char* wildcardedInput, char* outputBuffer, size_t outputBufferSize, struct wildcard* wildcard)
{
    struct stringListNode* wildcardStaticWords = wildcard->staticWords;
    char* currentWildcardStaticWord;
    size_t currentWildcardStaticWordLength;
    size_t outputBufferLength = 0;
    char* wildcardedInputCursor = wildcardedInput;
    size_t currentInputLength;
    bool firstRun = true;
    bool stop = false;

    outputBuffer[0] = '\0';

    if (isFromThisWildcard(wildcardedInput, wildcard) == true)
    {
        while ((stringList_hasNext(wildcardStaticWords) == true) && (outputBufferLength < outputBufferSize) && (stop == false))
        {
            currentWildcardStaticWord = stringList_next(&wildcardStaticWords);
            currentWildcardStaticWordLength = strlen(currentWildcardStaticWord);

            if ((firstRun == true) && (wildcard->begin == STATIC_WORD))
            {
                wildcardedInputCursor += currentWildcardStaticWordLength;

                firstRun = false;
            }
            else
            {
                currentInputLength = 0;

                while ((strBeginsWith(currentWildcardStaticWord, wildcardedInputCursor) == false) && (currentInputLength < outputBufferSize))
                {
                    wildcardedInputCursor++;
                    currentInputLength++;
                }

                if ((outputBufferLength + currentInputLength + 1) < outputBufferSize) // +1 for '\n'
                {
                    strncpy(outputBuffer + outputBufferLength, wildcardedInputCursor - currentInputLength, currentInputLength);

                    outputBufferLength += currentInputLength + 1;

                    outputBuffer[outputBufferLength - 1] = '\n';
                }
                else
                {
                    stop = true;
                }

                wildcardedInputCursor += currentWildcardStaticWordLength;
            }
        }
    }
}

bool isFromThisWildcard(char* input, struct wildcard* wildcard)
{
    struct stringListNode* wildcardStaticWords = wildcard->staticWords;
    bool result = true;
    char* currentWildcardStaticWord;
    char* inputCursor = input;
    size_t inputLength = strlen(input);
    size_t inputCursorPosition = 0;
    bool matchWithCurrentStaticWord;
    size_t currentWildcardStaticWordLength;
    struct stringListListNode* currentStarExclusions = wildcard->starExclusions;
    struct stringListNode* currentStarExclusion;

    while ((result == true) && (inputCursorPosition < inputLength) && (stringList_hasNext(wildcardStaticWords) == true))
    {
        currentWildcardStaticWord = stringList_next(&wildcardStaticWords);
        matchWithCurrentStaticWord = strBeginsWith(currentWildcardStaticWord, inputCursor);

        if (stringListList_hasNext(currentStarExclusions))
        {
            currentStarExclusion = stringListList_next(&currentStarExclusions);
        }
        else
        {
            currentStarExclusion = NULL;
        }

        while ((result == true) && (inputCursorPosition < inputLength) && (matchWithCurrentStaticWord == false))
        {
            inputCursorPosition++;
            inputCursor++;

            if ((currentStarExclusion != NULL) && (currentStarExclusion->nodeContent != NULL) && (stringList_stringBeginsWithAtLeastOneStringOfThisStringList(inputCursor, currentStarExclusion) == true))
            {
                result = false;
            }

            if (strcmp(currentWildcardStaticWord, "\n") != 0)
            {
                matchWithCurrentStaticWord = strBeginsWith(currentWildcardStaticWord, inputCursor);
            }
        }

        if (matchWithCurrentStaticWord == true)
        {
            currentWildcardStaticWordLength = strlen(currentWildcardStaticWord);

            inputCursor += currentWildcardStaticWordLength;
            inputCursorPosition += currentWildcardStaticWordLength;
        }
    }

    if ((result == true) && (stringList_hasNext(wildcardStaticWords) == true)) // If there are remaining static words in the list
    {
        result = false;
    }

    return result;
}

struct wildcard* getAssociatedWildcard(char* wildcardedInput, struct wildcardListNode* wildcards)
{
    struct wildcardListNode* wildcardsList = wildcards;
    struct wildcard* currentWildcard;
    struct wildcard* result = NULL;

    while ((wildcardList_hasNext(wildcardsList) == true) && (result == NULL))
    {
        currentWildcard = wildcardList_next(&wildcardsList);

        if (isFromThisWildcard(wildcardedInput, currentWildcard) == true)
        {
            result = currentWildcard;
        }
    }

    return result;
}

size_t lengthNeededToFillWildcard(char* input, struct wildcard* wildcard)
{
    size_t result = strlen(input);
    struct stringListNode* currentWildcardStaticWordsList = wildcard->staticWords;
    char* currentWildcardStaticWord;

    while (stringList_hasNext(currentWildcardStaticWordsList) == true)
    {
        currentWildcardStaticWord = stringList_next(&currentWildcardStaticWordsList);

        result += strlen(currentWildcardStaticWord);
    }

    return result;
}

bool stringList2WildcardWithStringExclusions(char* outputBuffer, size_t outputBufferSize, struct stringListNode* strings)
{
    size_t numberOfStrings = stringList_length(strings);
    size_t neededSize = stringList_allStringsLength(strings) + (numberOfStrings - 1) + 5;
    bool result = true;
    char* outputBufferCursor = outputBuffer + 3;
    struct stringListNode* currentStringListNode = strings;
    char* currentString;
    size_t currentStringLength;

    if (neededSize > outputBufferSize)
    {
        result = false;
    }
    else
    {
        outputBuffer[0] = '*';
        outputBuffer[1] = '-';
        outputBuffer[2] = '{';

        while (stringList_hasNext(currentStringListNode) == true)
        {
            currentString = stringList_next(&currentStringListNode);

            if (currentString != (char*)NULL)
            {
                currentStringLength = strlen(currentString);

                memcpy(outputBufferCursor, currentString, currentStringLength);

                outputBufferCursor += currentStringLength;

                if (stringList_hasNext(currentStringListNode) == true)
                {
                    *outputBufferCursor = ',';
                    outputBufferCursor++;
                }
            }
        }

        outputBufferCursor[0] = '}';
        outputBufferCursor[1] = '\0';
    }

    return result;
}

size_t getNumberOfWildcardsInWildcard(struct wildcard* wildcard)
{
    size_t result = 0;

    result += stringListList_length(wildcard->starExclusions);

    return result;
}