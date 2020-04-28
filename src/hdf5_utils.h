#ifndef HDF5_UTILS_H
#define HDF5_UTILS_H

#include "hdf5.h"
#include "k.h"

// HDF5 Types
#define HDF5FLOAT H5T_NATIVE_DOUBLE 
#define HDF5INT   H5T_NATIVE_INT
#define HDF5LONG  H5T_NATIVE_LONG
#define HDF5REAL  H5T_NATIVE_FLOAT
#define HDF5SHORT H5T_NATIVE_SHORT
#define HDF5CHAR  H5T_C_S1
#define HDF5BYTE  H5T_NATIVE_UCHAR

// k datatypes
typedef enum {NUMERIC, STRING, INVALID} kdata_t;

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy));

// ktype (char) to k typegroup
kdata_t checkvalid(char ktype);

// ktype (char) to hdf5 numeric types
hid_t hdf5typ_from_k(char ktype);

// Disable errors from hdf5 side
void disableErr(void);

// Create NUMERIC dataset
int createNumericDataset(hid_t file, char *dataname, K kdims, K ktype);

// Create STRING dataset
int createStringDataset(hid_t file, char *dataname, K kdims);

// Create NUMERIC attribute
int createNumericAttribute(hid_t data, char *attrname, K kdims, K ktype);

// Create STRING attribute
int createStringAttribute(hid_t data, char *attrname, K kdims);

// Check that the dataset exists
int checkdataset(hid_t file, char *dataname);

int checkgroup(hid_t file, char *groupname);

// open group/dataset object (depending on object type)
hid_t openGroupData(hid_t file, char *dataname);

// close group/dataset object (depending on object type)
void closeGroupData(hid_t file, char *dataname, hid_t data);

#endif // HDF5_UTILS_H
