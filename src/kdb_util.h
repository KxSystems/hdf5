#include "k.h"

#define KNL (K) 0

#define HDF5FLOAT H5T_NATIVE_DOUBLE 
#define HDF5INT   H5T_NATIVE_INT
#define HDF5LONG  H5T_NATIVE_LONG
#define HDF5REAL  H5T_NATIVE_FLOAT
#define HDF5SHORT H5T_NATIVE_SHORT
#define HDF5CHAR  H5T_C_S1
#define HDF5BYTE  H5T_NATIVE_UCHAR

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

// get k string or symbol name
char * getkstring(K x);

// check types
int checkType(const C* tc, ...);

// create dictionaries as key, val, key, val ...
K xd0(I n, ...); 
#define xd(...) xd0(0, __VA_ARGS__, (S) 0)
