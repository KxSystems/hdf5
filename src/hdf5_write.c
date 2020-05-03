#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// declare write utils
K writeNumeric(K dset, hid_t loc, hid_t space, hid_t ntype);
K writeString(K dset, hid_t loc, hid_t space, hid_t ntype);

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Ii]c", fname, dname, kdims, ktype))
    return KNL;
  hid_t file, data, space;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname;
  hssize_t rank, i;
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
  if(htype != ntype)
    return krr((S)"type does not match");
  space = H5Dget_space(data);
  if(space < 0)
    return krr((S)"error opening dataspace");
  // check rank
  rank = kdims->n;
  if(rank != H5Sget_simple_extent_npoints(space))
    return krr((S)"rank does no match");
  // check dims
  dims = calloc(rank, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < rank; i++)
    if(kJ(kdims)[i] != (J)dims[i])
      return krr((S)"dimensions do not match");
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    writeNumeric(dset, data, space, ntype);
  else if(gtype == STRING)
    writeString(dset, data, space, ntype);
  else{
    krr((S)"unsupported datatype");
  }
  H5Dclose(data);
  H5Sclose(space);
  return KNL;
}

EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Cs][Ii]c", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data, attr, space;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname, *attrname;
  hssize_t rank, i;
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
  dtype = H5Dget_type(attr);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  if(htype != ntype)
    return krr((S)"type does not match");
  space = H5Aget_space(attr);
  if(space < 0)
    return krr((S)"error opening dataspace");
  // check rank
  rank = kdims->n;
  if(rank != H5Sget_simple_extent_npoints(space))
    return krr((S)"rank does no match");
  // check dims
  dims = calloc(rank, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < rank; i++)
    if(kJ(kdims)[i] != (J)dims[i])
      return krr((S)"dimensions do not match");
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    writeNumeric(dset, attr, space, ntype);
  else if(gtype == STRING)
    writeString(dset, attr, space, ntype);
  else{
    krr((S)"unsupported datatype");
  }
  H5Aclose(attr);
  H5Sclose(space);
  return KNL;
}

// define write utils

long copyNumeric(hid_t UNUSED(loc), K UNUSED(dset), void *UNUSED(dset_data), hid_t UNUSED(ntype), long idx){
/*
  int i, j;
  if(idx < 0)
    return idx;
  if(dset->t == KG){
    for(i = 0; i < dset->n; ++i)
      dset_data[idx + i] = kG(dset)[i];
    idx += dset->n;
  }
  else if(dset->t == 0){
    for(i = 0; i < dset->n; ++i)
  }
  else
    return -1;
*/
  return idx;
}

K writeNumeric(K dset, hid_t loc, hid_t space, hid_t ntype){
  hid_t status;
  long npoints, idx;
  void *dset_data;
  npoints = H5Sget_simple_extent_npoints(space);
  dset_data = calloc(npoints, sizeof(double)); // need size of ntype
  idx = copyNumeric(loc, dset, dset_data, ntype, 0);
  if(idx != npoints)
    return krr("invalid data");
  status = H5Dwrite(loc, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data); // need generic write func
  if(status < 0)
    return krr("error writing to dataset");
  free(dset_data);
  return KNL;
}

K writeString(K UNUSED(dset), hid_t UNUSED(loc), hid_t UNUSED(space), hid_t UNUSED(ntype)){
/*
  long i,arr;
  // Create a variable length memory space to store the K chars
  hid_t memtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(memtype, H5T_VARIABLE);
  arr = dset->n;
  char **wdata = malloc(sizeof(char*) * arr);
  for(i=0;i<arr;i++)
    wdata[i]= kdbGetString(kK(dset)[i]);
  if(strcmp(data_attr,"d")==0)
    H5Dwrite(data, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
  else
    H5Awrite(data, memtype, wdata);
  free(wdata);
  H5Tclose(memtype);
*/
  return KNL;
}
