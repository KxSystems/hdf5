#ifndef KDB_UTILS_H
#define KDB_UTILS_H

#include "k.h"

#ifdef __GNUC__
#  define UNUSED(x) x __attribute__((__unused__))
#else
#  define UNUSED(x) x
#endif

#ifdef _WIN32
#  define EXP __declspec(dllexport)
#else
#  define EXP
#endif

#define KNL (K) 0

// get string from kdb char, string or symbol
char * kdbGetString(K x);

// check types of args
int kdbCheckType(const char *typePattern, ...);

// create dictionary (key, val, key, val, ...)
K kdbCreateDict0(void *dummy, ...);
#define kdbCreateDict(...) kdbCreateDict0(0, __VA_ARGS__, (S)0)

#endif // KDB_UTILS_H
