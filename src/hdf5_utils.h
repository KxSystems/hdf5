#ifndef HDF5_UTILS_H
#define HDF5_UTILS_H

#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"

// string types
extern hid_t varstringtype;
void initvarstringtype();

// error handler info
extern herr_t (*err_func)(void*);
extern void *err_data;
void initerror();
void errorOn();
void errorOff();

// manipulate attribute functions to have the same signature as equivalent dataset functions
typedef hid_t (*createfunc_t)(hid_t, const char *, hid_t, hid_t, hid_t, hid_t, hid_t);
typedef herr_t (*readfunc_t)(hid_t, hid_t, hid_t, hid_t, hid_t, void *);
typedef herr_t (*writefunc_t)(hid_t, hid_t, hid_t, hid_t, hid_t, const void *);
typedef herr_t (*closefunc_t)(hid_t);

hid_t kdbH5Acreate(hid_t attr, const char *name, hid_t type, hid_t space, hid_t UNUSED(lcpl), hid_t cpl, hid_t apl);
herr_t kdbH5Aread(hid_t attr, hid_t memtype, hid_t mspace, hid_t fspace, hid_t pl, void *buf);
herr_t kdbH5Awrite(hid_t attr, hid_t memtype, hid_t mspace, hid_t fspace, hid_t pl, const void *buf);

// open functions
hid_t kdbH5Fopen(K name, unsigned flags);
hid_t kdbH5Dopen(hid_t loc, K name);
hid_t kdbH5Aopen(hid_t loc, K name);
hid_t kdbH5Oopen(hid_t loc, K name);

// k datatypes
typedef enum {NUMERIC, STRING, INVALID} ktypegroup_t;

// htype (hid_t) to ktype (H)
H h2kType(hid_t htype);

// ktype (char) to htype (hid_t)
hid_t k2hType(char ktype);

// ktype (char) to ktypegroup (ktypegroup_t)
ktypegroup_t getKTypeGroup(char ktype);

#endif // HDF5_UTILS_H
