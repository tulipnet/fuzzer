#include "utils.h"

size_t min(size_t a, size_t b)
{
    size_t result;

    if (a < b)
    {
        result = a;
    }
    else
    {
        result = b;
    }

    return result;
}

size_t max(size_t a, size_t b)
{
    size_t result;

    if (a > b)
    {
        result = a;
    }
    else
    {
        result = b;
    }

    return result;
}

void strConcat(char* dst, char* src, size_t len)
{
    size_t srcLen = strnlen(src, len);
    size_t dstLen = strnlen(dst, len);
    size_t i = 0;

    while ((i < srcLen) && ((dstLen + i) < (len - 1)))
    {
        dst[dstLen + i] = src[i];
        i++;
    }
}

void strReset(char* str, size_t len)
{
    size_t i;

    for (i = 0; i < len; ++i)
    {
        str[i] = '\0';
    }
}

void strConcatLeft(char* dst, char* src, size_t len)
{
    size_t srcLen = strlen(src);
    size_t dstLen = strlen(dst);
    size_t i = dstLen;

    if (srcLen + dstLen <= len)
    {
        // Right shift of the begining of dst by the length of the src
        if (i > 0)
        {
            do
            {
                dst[srcLen + i] = dst[i];
                --i;
            } while (i > 0);
        }

        // Replacement of the begining of dst by src
        i = 0;

        while (i < srcLen)
        {
            dst[i] = src[i];
            ++i;
        }
    }
}

bool strIsEmpty(char* str)
{
    bool result;

    if (str[0] == '\0')
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

bool strBeginsWith(char* prefix, char* str)
{
    size_t prefixLength = strlen(prefix);

    return strncmp(prefix, str, prefixLength) == 0;
}

void printStrWithIdAndDelim(char* str, int id)
{
    printf("id = %d\n%s\n-------------\n", id, str);
}