#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// disable errors from hdf5 side
void disableErr(void){H5Eset_auto1(NULL,NULL);}

// htype (hid_t) to ktype (H)
H h2kType(hid_t htype){
  hid_t ntype;
  H result;
  ntype = H5Tget_native_type(htype, H5T_DIR_ASCEND);
       if(H5Tequal(ntype, H5T_NATIVE_CHAR))
    result = KC;
  else if(H5Tequal(ntype, H5T_NATIVE_SHORT))
    result = KH;
  else if(H5Tequal(ntype, H5T_NATIVE_INT))
    result = KI;
  else if(H5Tequal(ntype, H5T_NATIVE_LONG))
    result = (sizeof(long) == 16) ? KI : KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_LLONG))
    result = KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_UCHAR))
    result = KC;
  else if(H5Tequal(ntype, H5T_NATIVE_USHORT))
    result = KH;
  else if(H5Tequal(ntype, H5T_NATIVE_UINT))
    result = KI;
  else if(H5Tequal(ntype, H5T_NATIVE_ULONG))
    result = (sizeof(long) == 16) ? KI : KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_ULLONG))
    result = KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_FLOAT))
    result = KE;
  else if(H5Tequal(ntype, H5T_NATIVE_DOUBLE))
    result = KF;
  else if(H5Tequal(ntype, H5T_NATIVE_B8))
    result = KF;
  else if(H5Tequal(ntype, H5T_NATIVE_B16))
    result = KF;
  else if(H5Tequal(ntype, H5T_NATIVE_B32))
    result = KF;
  else if(H5Tequal(ntype, H5T_NATIVE_B64))
    result = KF;
   else
     result = 0;
  H5Tclose(ntype);
  return result;
}

// ktype (char) to htype (hid_t)
hid_t k2hType(char ktype){
  switch(ktype){
    case 'b':
    case 'x':
      return H5T_NATIVE_UCHAR;
    case 'h':
      return H5T_NATIVE_SHORT;
    case 'i':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      return H5T_NATIVE_INT;
    case 'j':
    case 'p':
    case 'n':
      return H5T_NATIVE_LLONG;
    case 'e':
      return H5T_NATIVE_FLOAT;
    case 'f':
    case 'z':
      return H5T_NATIVE_DOUBLE;
    default:
      return 0;
  }
}

// ktype (char) to ktypegroup (ktypegroup_t)
ktypegroup_t getKTypeGroup(char ktype){
  if(NULL != strchr("hijfebxpmdznuvt", ktype))
    return NUMERIC;
  if(NULL != strchr("csg", ktype))
    return STRING;
  return INVALID;
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
  dtype = k2hType(ktype->g);
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
  dtype = k2hType(ktype->g);
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

// check if dataset exists
int checkDataset(hid_t file, char *dataname){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  return(H5G_DATASET == statbuf.type)? 1 : 0;
}
