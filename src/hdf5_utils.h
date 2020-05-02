#ifndef HDF5_UTILS_H
#define HDF5_UTILS_H

#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"

// manipulate attribute functions to have the same signature as equivalent dataset functions
typedef hid_t (*createfunc_t)(hid_t, const char *, hid_t, hid_t, hid_t, hid_t, hid_t);
typedef herr_t (*readfunc_t)(hid_t, hid_t, hid_t, hid_t, hid_t, void *);
typedef herr_t (*writefunc_t)(hid_t, hid_t, hid_t, hid_t, hid_t, const void *);
typedef herr_t (*closefunc_t)(hid_t);

hid_t kdbH5Acreate(hid_t, const char *, hid_t, hid_t, hid_t, hid_t, hid_t);
herr_t kdbH5Aread(hid_t, hid_t, hid_t, hid_t, hid_t, void *);
herr_t kdbH5Awrite(hid_t, hid_t, hid_t, hid_t, hid_t, const void *);

// HDF5 Types
#define HDF5FLOAT H5T_NATIVE_DOUBLE 
#define HDF5INT   H5T_NATIVE_INT
#define HDF5LONG  H5T_NATIVE_LONG
#define HDF5REAL  H5T_NATIVE_FLOAT
#define HDF5SHORT H5T_NATIVE_SHORT
#define HDF5BYTE  H5T_NATIVE_UCHAR

// k datatypes
typedef enum {NUMERIC, STRING, INVALID} ktypegroup_t;

// Disable errors from hdf5 side
void disableErr(void);

// htype (hid_t) to ktype (H)
H h2kType(hid_t htype);

// ktype (char) to htype (hid_t)
hid_t k2hType(char ktype);

// ktype (char) to ktypegroup (ktypegroup_t)
ktypegroup_t getKTypeGroup(char ktype);

// create NUMERIC dataset
K createNumericDataset(hid_t file, char *dataname, K kdims, K ktype);

// create STRING dataset
K createStringDataset(hid_t file, char *dataname, K kdims);

// create NUMERIC attribute
K createNumericAttribute(hid_t data, char *attrname, K kdims, K ktype);

// create STRING attribute
K createStringAttribute(hid_t data, char *attrname, K kdims);

// Check that the dataset exists
int checkDataset(hid_t file, char *dataname);

#endif // HDF5_UTILS_H
