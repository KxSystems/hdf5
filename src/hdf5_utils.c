/* --- HDF5 utility functions --- */

#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy)){
  disable_err();
  return KNL;
}

// ktype (char) to k typegroup
kdata_t checkvalid(char ktype){
  if(NULL != strchr("hijfebxpmdznuvt", ktype))
    return NUMERIC;
  if(NULL != strchr("csg", ktype))
    return STRING;
  return INVALID;
}

// ktype (char) to hdf5 numeric types
hid_t hdf5typ_from_k(char ktype){
  switch(ktype){
    case 'h':
      return HDF5SHORT;
    case 'i':
    case 'b':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      return HDF5INT;
    case 'j':
    case 'p':
    case 'n':
      return HDF5LONG;
    case 'f':
    case 'z':
      return HDF5FLOAT;
    case 'e':
      return HDF5REAL;
    case 'x':
      return H5T_NATIVE_UCHAR;
    default:
      return 0;
  }
}

// Disable errors from hdf5 side
void disable_err(void){H5Eset_auto1(NULL,NULL);}

// Create NUMERIC dataset
int createsimpledataset(hid_t file, char *dataname, K kdims, K ktype){
  static hsize_t dims[3];
  hid_t space, dtype;
  int i, rank = kdims->n;
  if(rank > 3)
    return 0;
  for(i = 0; i < rank; ++i)
    dims[i] = kI(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  dtype = H5Tcopy(hdf5typ_from_k(ktype->g));
  H5Tset_order(dtype, H5T_ORDER_LE);
  H5Dcreate(file, dataname, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(dtype);
  return 1;
}

// Create STRING dataset
int createstrdataset(hid_t file, char *dataname, K kdims){
  hid_t space, filetype;
  hsize_t dims[1];
  int klen = kI(kdims)[0];
  dims[0] = klen;
  filetype = H5Tcopy(H5T_FORTRAN_S1);
  H5Tset_size(filetype, H5T_VARIABLE);
  space = H5Screate_simple(1, dims, NULL);
  H5Dcreate(file, dataname, filetype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(filetype);
  return 1;
}

// Create NUMERIC attribute
int createsimpleattr(hid_t data, char *attrname, K kdims, K ktype){
  int i, rank;
  hid_t space;
  if(-KI==kdims->t)
    rank = 1;
  else
    rank = (I)kdims->n;
  // System limited to 3-D in current iteration
  if(rank>3)
    return 0;
  hsize_t dims[rank];
  for(i=0;i < rank;i++)
    dims[i] = kI(kdims)[i];
  // Define dimensionality of the space to be created
  space = H5Screate(H5S_SIMPLE);
  H5Sset_extent_simple(space, rank, dims, NULL);
  // Create the attribute dataset of appropriate type
  H5Acreate2(data, attrname, hdf5typ_from_k(ktype->g), space, H5P_DEFAULT, H5P_DEFAULT);
  // Clean up
  H5Sclose(space);
  return 1;
}

// Create STRING attribute
int createstrattr(hid_t data, char *attrname, K kdims){
  hid_t space, filetype;
  hsize_t dims[1];
  // Number of char arrays or symbols to within the string attribute
  int klen = kI(kdims)[0];
  dims[0] = klen;
  // Write data to FORTRAN type (this handles null termination)
  filetype = H5Tcopy(H5T_FORTRAN_S1);
  H5Tset_size(filetype, H5T_VARIABLE);
  space = H5Screate_simple(1, dims, NULL);
  H5Acreate2(data, attrname, filetype, space, H5P_DEFAULT,  H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(filetype);
  return 1;
}

// Check that the dataset exists
int checkdataset(hid_t file, char *dataname){
  // Create a buffer for datatype
  H5G_stat_t statbuf;
  // Commit the data information to the buffer
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  // Check the type of data is a dataset
  if(H5G_DATASET == statbuf.type)
    return 1;
  else
    return 0;
}

int checkgroup(hid_t file, char *groupname){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, groupname, 0, &statbuf);
  if(H5G_GROUP == statbuf.type)
    return 1;
  else
    return 0;
}

// Returns group/data object depending on format of the file denoted in dataname
hid_t isGroupData(hid_t file, char *dataname){
  hid_t data;
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  switch(statbuf.type){
  case H5G_GROUP:
    data = H5Gopen(file, dataname, H5P_DEFAULT);
    break;
  case H5G_DATASET:
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    break;
  default:
    data = 0;
    break;
  }
  return data;
}

// Close the group or datatype depending on object type
void closeGroupData(hid_t file, char *dataname,hid_t data){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  switch(statbuf.type){
    case H5G_GROUP:
      H5Gclose(data);
      break;
    case H5G_DATASET:
      H5Dclose(data);
      break;
    default:
      break;
  }
}
