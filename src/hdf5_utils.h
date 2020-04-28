#ifndef HDF5_UTILS_H
#define HDF5_UTILS_H

#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"

// HDF5 Types
#define HDF5FLOAT H5T_NATIVE_DOUBLE 
#define HDF5INT   H5T_NATIVE_INT
#define HDF5LONG  H5T_NATIVE_LONG
#define HDF5REAL  H5T_NATIVE_FLOAT
#define HDF5SHORT H5T_NATIVE_SHORT
#define HDF5CHAR  H5T_C_S1
#define HDF5BYTE  H5T_NATIVE_UCHAR

// k datatypes
typedef enum {NUMERIC, STRING, INVALID} ktype_t;

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy));

// Disable errors from hdf5 side
void disableErr(void);

// ktype (char) to k type
ktype_t getKType(char ktype);

// ktype (char) to hdf5 numeric type
hid_t getHDF5Type(char ktype);

// create NUMERIC dataset
int createNumericDataset(hid_t file, char *dataname, K kdims, K ktype);

// create STRING dataset
int createStringDataset(hid_t file, char *dataname, K kdims);

// create NUMERIC attribute
int createNumericAttribute(hid_t data, char *attrname, K kdims, K ktype);

// create STRING attribute
int createStringAttribute(hid_t data, char *attrname, K kdims);

// Check that the dataset exists
int checkDataset(hid_t file, char *dataname);

#endif // HDF5_UTILS_H
