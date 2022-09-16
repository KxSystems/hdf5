#include <stdlib.h>
#include <string.h>
#include "hdf5_utils.h"

// declare read utils
K readData(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataSimple(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataSimpleNumeric(hid_t dset, hsize_t npoints, hid_t ntype, hid_t space, readfunc_t readfunc);
K readDataSimpleString(hid_t dset, hsize_t npoints, hid_t ntype, hid_t space, readfunc_t readfunc);
K readDataCompound(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataCompoundNumeric(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t space, readfunc_t readfunc);
K readDataCompoundString(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t space, readfunc_t readfunc);

EXP K hdf5readDataset(K fname, K dname){
  if(!kdbCheckType("CC", fname, dname))
    return KNL;
  K result;
  hid_t file, data, space, dtype;
  file = kdbH5Fopen(fname, H5F_ACC_RDONLY);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Dopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset");
  space = H5Dget_space(data);
  if(space < 0){
    H5Dclose(data);
    return krr((S)"error opening dataspace");
  }
  dtype = H5Dget_type(data);
  if(dtype < 0){
    H5Dclose(data);
    H5Sclose(space);
    return krr((S)"error retrieving dataset type");
  }
  result = readData(data, space, dtype, H5Dread);
  H5Dclose(data);
  H5Sclose(space);
  H5Tclose(dtype);
  return result;
}

EXP K hdf5readAttrDataset(K fname, K dname, K aname){
  if(!kdbCheckType("CCC", fname, dname, aname))
    return KNL;
  K result;
  hid_t file, data, attr, space, dtype;
  file = kdbH5Fopen(fname, H5F_ACC_RDONLY);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Oopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  attr = kdbH5Aopen(data, aname);
  H5Oclose(data);
  if(attr < 0)
    return krr((S)"error opening attribute");
  space = H5Aget_space(attr);
  if(space < 0){
    H5Aclose(attr);
    return krr((S)"error opening dataspace");
  }
  dtype = H5Aget_type(attr);
  if(dtype < 0){
    H5Aclose(attr);
    H5Sclose(space);
    return krr((S)"error retrieving attribute type");
  }
  result = readData(attr, space, dtype, kdbH5Aread);
  H5Aclose(attr);
  H5Sclose(space);
  H5Tclose(dtype);
  return result;
}

// define read utils

K readData(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc){
  K result;
  H5T_class_t dclass;
  dclass = H5Tget_class(dtype);
  result = (dclass == H5T_COMPOUND) ?
    readDataCompound(dset, space, dtype, readfunc):
    readDataSimple  (dset, space, dtype, readfunc);
  return result;
}

K readDataSimple(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc){
  K result;
  hid_t ntype;
  hsize_t npoints;
  H5T_class_t nclass;
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  npoints = H5Sget_simple_extent_npoints(space);
  nclass = H5Tget_class(ntype);
  result = (nclass == H5T_STRING) ?
    readDataSimpleString(dset, npoints, ntype, space, readfunc):
    readDataSimpleNumeric(dset, npoints, ntype, space, readfunc);
  H5Tclose(ntype);
  return result;
}

K readDataSimpleNumeric(hid_t dset, hsize_t npoints, hid_t ntype, hid_t UNUSED(space), readfunc_t readfunc){
  K result;
  result = ktn(h2kType(ntype), npoints);
  readfunc(dset, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, kG(result));
  return result;
}

K readDataSimpleString(hid_t dset, hsize_t npoints, hid_t ntype, hid_t space, readfunc_t readfunc){
  K result;
  hid_t stype;
  char **vdata, *fdata;
  unsigned i;
  stype = H5Tcopy(H5T_C_S1);
  result = ktn(0,npoints);
  // variable length string
  if(H5Tis_variable_str(ntype)){
    H5Tset_size(stype, H5T_VARIABLE);
    vdata = (char **)calloc(npoints, sizeof(char *));
    readfunc(dset, stype, H5S_ALL, H5S_ALL, H5P_DEFAULT, vdata);
    for(i = 0; i < npoints; ++i)
      kK(result)[i] = kp(vdata[i]);
    H5Dvlen_reclaim(stype, space, H5P_DEFAULT, vdata);
    free(vdata);
  }
  // fixed length string
  else{
    size_t sz = H5Tget_size(ntype) + 1;
    H5Tset_size(stype, sz);
    fdata = (char *)calloc(npoints * sz, sizeof(char));
    readfunc(dset, stype, H5S_ALL, H5S_ALL, H5P_DEFAULT, fdata);
    for(i = 0; i < npoints; ++i)
      kK(result)[i] = kp(fdata + i * sz);
    free(fdata);
  }
  H5Tclose(stype);
  return result;
}

K readDataCompound(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc){
  hsize_t *dims;
  hid_t ntype, mtype;
  H5T_class_t mclass;
  char* mname;
  int i, nmembers, rank;
  K keys, vals, mval;
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  nmembers = H5Tget_nmembers(ntype);
  rank  = H5Sget_simple_extent_ndims(space);
  dims  = calloc(rank, sizeof(hsize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  keys = ktn(KS,nmembers);
  vals = ktn(0,nmembers);
  for(i = 0; i < nmembers; ++i){
    mname = H5Tget_member_name(ntype, i);
    kS(keys)[i] = ss(mname);
    mclass = H5Tget_member_class(ntype, i);
    mtype  = H5Tget_member_type(ntype, i);
    mval = (mclass == H5T_STRING) ?
      readDataCompoundString(dset, mname, dims[0], mtype, space, readfunc):
      readDataCompoundNumeric (dset, mname, dims[0], mtype, space, readfunc);
    kK(vals)[i] = mval;
    H5Tclose(mtype);
    H5free_memory(mname);
  }
  free(dims);
  H5Tclose(ntype);
  return(xT(xD(keys,vals)));
}

K readDataCompoundNumeric(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t UNUSED(space), readfunc_t readfunc){
  K result;
  hid_t memtype;
  memtype = H5Tcreate(H5T_COMPOUND, H5Tget_size(mtype));
  H5Tinsert(memtype, mname, 0, mtype);
  result = ktn(h2kType(mtype), npoints);
  readfunc(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, kG(result));
  H5Tclose(memtype);
  return result;
}

K readDataCompoundString(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t space, readfunc_t readfunc){
  hid_t memtype, stype;
  K result;
  char **vdata, *fdata;
  unsigned i;
  stype = H5Tcopy(H5T_C_S1);
  result = ktn(0,npoints);
  // variable length string
  if(H5Tis_variable_str(mtype)){
    H5Tset_size(stype, H5T_VARIABLE);
    memtype = H5Tcreate(H5T_COMPOUND, sizeof(char *));
    H5Tinsert(memtype, mname, 0, stype);
    vdata = (char **)calloc(npoints, sizeof(char *));
    readfunc(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, vdata);
    for(i = 0; i < npoints; ++i)
      kK(result)[i] = kp(vdata[i]);
    H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, vdata);
    free(vdata);
  }
  // fixed length string
  else{
    size_t sz = H5Tget_size(mtype) + 1;
    H5Tset_size(stype, sz);
    memtype = H5Tcreate(H5T_COMPOUND, sz);
    H5Tinsert(memtype, mname, 0, stype);
    fdata = (char *)calloc(npoints * sz, sizeof(char));
    readfunc(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, fdata);
    for(i = 0; i < npoints; ++i)
      kK(result)[i] = kp(fdata + i * sz);
    free(fdata);
  }
  H5Tclose(stype);
  H5Tclose(memtype);
  return result;
}
