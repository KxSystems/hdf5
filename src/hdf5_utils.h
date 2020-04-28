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

// Retrieve hdf5 numeric types
hid_t hdf5typ_from_k(K ktype);

// Disable errors from hdf5 side
void disable_err(void);

// Create a string attribute
int createstrattr(hid_t data, char *attrname, K kdims);

// Used for the creation of simple attributes for types ijhef
int createsimpleattr(hid_t data, char *attrname, K kdims, K ktype);

int createstrdataset(hid_t file, char *dataname, K kdims);

// Used for the creation of simple datasets of type ijhef
int createsimpledataset(hid_t file, char *dataname, K kdims, K ktype);

// Check that the dataset exists
int checkdataset(hid_t file, char *dataname);

int checkgroup(hid_t file, char *groupname);

// Returns group/data object depending on format of the file denoted in dataname
hid_t isGroupData(hid_t file, char *dataname);

// Close the group or datatype depending on object type
void closeGroupData(hid_t file, char *dataname,hid_t data);

// used to check what datatype is being passed in to make decisions on write path
kdata_t checkvalid(char ktype);

#endif // HDF5_UTILS_H
