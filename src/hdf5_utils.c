/* --- HDF5 utility functions --- */

#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy)){
  disableErr();
  return KNL;
}

// disable errors from hdf5 side
void disableErr(void){H5Eset_auto1(NULL,NULL);}

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

// create NUMERIC dataset
int createNumericDataset(hid_t file, char *dataname, K kdims, K ktype){
  hsize_t dims[3];
  hid_t space, dtype;
  int i, rank = kdims->n;
  if(rank > 3){
    krr("numerical datasets must have dimensionality <= 3");
    return 1;
  }
  for(i = 0; i < rank; ++i)
    dims[i] = kI(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  dtype = hdf5typ_from_k(ktype->g);
  H5Dcreate(file, dataname, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  return 0;
}

// create STRING dataset
int createStringDataset(hid_t file, char *dataname, K kdims){
  hsize_t dims[1];
  hid_t space, dtype;
  dims[0] = kI(kdims)[0];
  space = H5Screate_simple(1, dims, NULL);
  dtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(dtype, H5T_VARIABLE);
  H5Dcreate(file, dataname, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(dtype);
  return 0;
}

// create NUMERIC attribute
int createNumericAttribute(hid_t data, char *attrname, K kdims, K ktype){
  hsize_t dims[3];
  hid_t space, dtype;
  int i, rank = kdims->n;
  if(rank > 3){
    krr("numerical attributes must have dimensionality <= 3");
    return 1;
  }
  for(i = 0; i < rank; ++i)
    dims[i] = kI(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  dtype = hdf5typ_from_k(ktype->g);
  H5Acreate(data, attrname, dtype, space, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  return 0;
}

// create STRING attribute
int createStringAttribute(hid_t data, char *attrname, K kdims){
  hsize_t dims[1];
  hid_t space, dtype;
  dims[0] = kI(kdims)[0];
  space = H5Screate_simple(1, dims, NULL);
  dtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(dtype, H5T_VARIABLE);
  H5Acreate(data, attrname, dtype, space, H5P_DEFAULT,  H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(dtype);
  return 0;
}

// open group/dataset object (depending on object type)
hid_t openGroupData(hid_t file, char *dataname){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  switch(statbuf.type){
  case H5G_GROUP:
    return H5Gopen(file, dataname, H5P_DEFAULT);
    break;
  case H5G_DATASET:
    return H5Dopen(file, dataname, H5P_DEFAULT);
    break;
  default:
    return 0;
  }
}

// close group/dataset object (depending on object type)
void closeGroupData(hid_t file, char *dataname, hid_t data){
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

// check if dataset exists
int checkDataset(hid_t file, char *dataname){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  return(H5G_DATASET == statbuf.type)? 1 : 0;
}
