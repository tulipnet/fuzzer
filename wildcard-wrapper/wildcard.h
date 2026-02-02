#ifndef WILDCARD_H
#define WILDCARD_H

enum wildcardBegin
{
    STAR,
    STATIC_WORD
};

struct wildcard
{
    struct stringListNode* staticWords;
    struct stringListListNode* starExclusions;
    enum wildcardBegin begin;
    __attribute__((objc_nullability("nullable"))) char* tag;
};

#endif