#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

size_t min(size_t a, size_t b);
size_t max(size_t a, size_t b);
void strConcat(char* dst, char* src, size_t len);
void strReset(char* str, size_t len);
void strConcatLeft(char* dst, char* src, size_t len);
bool strIsEmpty(char* str);
bool strBeginsWith(char* prefix, char* str);
void printStrWithIdAndDelim(char* str, int id);

#endif