#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// declare write utils
hid_t writeString(K dset, hid_t loc, writefunc_t write);

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  if(!kdbCheckType("CCJc", fname, dname, kdims, ktype))
    return KNL;
  hid_t file, data, space, status = -1;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname;
  hssize_t i;
  hsize_t *dims;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  dataname = kdbGetString(dname);
  data = H5Dopen(file, dataname, H5P_DEFAULT);
  free(dataname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset");
  // check datatypes
  htype = k2hType(ktype->g);
  dtype = H5Dget_type(data);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  H5Tclose(dtype);
  if(!H5Tequal(htype, ntype)){
    H5Dclose(data);
    H5Tclose(ntype);
    return krr((S)"type does not match");
  }
  space = H5Dget_space(data);
  if(space < 0){
    H5Dclose(data);
    H5Tclose(ntype);
    return krr((S)"error opening dataspace");
  }
  // check dims
  dims = calloc(kdims->n, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < kdims->n; i++){
    if(kJ(kdims)[i] != (J)dims[i]){
      free(dims);
      H5Dclose(data);
      H5Tclose(ntype);
      H5Sclose(space);
      return krr((S)"dimensions do not match");
    }
  }
  free(dims);
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    status = H5Dwrite(data, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, kG(dset));
  else if(gtype == STRING)
    writeString(dset, data, H5Dwrite);
  else{
    H5Dclose(data);
    H5Tclose(ntype);
    H5Sclose(space);
    krr((S)"unsupported datatype");
  }
  if(status < 0)
    krr((S)"error writing data");
  H5Dclose(data);
  H5Tclose(ntype);
  H5Sclose(space);
  return KNL;
}

EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  if(!kdbCheckType("CCCJc", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data, attr, space, status = -1;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname, *attrname;
  hssize_t i;
  hsize_t *dims;

  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  dataname = kdbGetString(dname);
  data = H5Oopen(file, dataname, H5P_DEFAULT);
  free(dataname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  attrname = kdbGetString(aname);
  attr = H5Aopen(data, attrname, H5P_DEFAULT);
  free(attrname);
  H5Oclose(data);
  if(attr < 0)
    return krr((S)"error opening attribute");
  // check datatypes
  htype = k2hType(ktype->g);
  dtype = H5Aget_type(attr);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  H5Tclose(dtype);
  if(!H5Tequal(htype, ntype)){
    H5Aclose(attr);
    H5Tclose(ntype);
    return krr((S)"type does not match");
  }
  space = H5Aget_space(attr);
  if(space < 0){
    H5Aclose(attr);
    H5Tclose(ntype);
    return krr((S)"error opening dataspace");
  }
  // check dims
  dims = calloc(kdims->n, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < kdims->n; i++){
    if(kJ(kdims)[i] != (J)dims[i]){
      free(dims);
      H5Aclose(attr);
      H5Tclose(ntype);
      H5Sclose(space);
      return krr((S)"dimensions do not match");
    }
  }
  free(dims);
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    status = kdbH5Awrite(attr, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, kG(dset));
  else if(gtype == STRING)
    status = writeString(dset, attr, kdbH5Awrite);
  else{
    H5Aclose(attr);
    H5Tclose(ntype);
    H5Sclose(space);
    return krr((S)"unsupported datatype");
  }
  if(status < 0)
    krr((S)"error writing data");
  H5Aclose(attr);
  H5Tclose(ntype);
  H5Sclose(space);
  return KNL;
}

// define write utils

hid_t writeString(K dset, hid_t loc, writefunc_t write){
  hid_t stype, status;
  int i;
  stype = H5Tcopy(H5T_C_S1);
  H5Tset_size(stype, H5T_VARIABLE);
  char **wdata = calloc(dset->n, sizeof(char*));
  for(i = 0; i < dset->n; ++i)
    wdata[i] = kdbGetString(kK(dset)[i]);
  status = write(loc, stype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
  for(i = 0; i < dset->n; ++i)
    free(wdata[i]);
  free(wdata);
  H5Tclose(stype);
  return status;
}
