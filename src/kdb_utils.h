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

// get k string or symbol name
char * getkstring(K x);

// check types
int checkType(const C* tc, ...);

// create dictionaries as key, val, key, val ...
K xd0(I n, ...); 
#define xd(...) xd0(0, __VA_ARGS__, (S) 0)

#endif // KDB_UTILS_H
