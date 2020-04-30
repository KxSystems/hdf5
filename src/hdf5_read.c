#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// declare read utils
typedef herr_t (*readfunc_t)(hid_t, hid_t, hid_t, hid_t, hid_t, void *);

K readData(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataSimple(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataCompound(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc);
K readDataCompoundValue(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t space, readfunc_t readfunc);
K readDataCompoundString(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t space, readfunc_t readfunc);

herr_t kdbH5Aread(hid_t data, hid_t memtype, hid_t UNUSED(ms), hid_t UNUSED(fs), hid_t UNUSED(pl), void *buf){
  return H5Aread(data, memtype, buf);
}

// read data from a dataset
EXP K hdf5readDataset(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K result;
  hid_t file, data;
  char *filename, *dataname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    return krr((S)"file does not exist");
  }
  dataname = kdbGetString(dname);
  data = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset does not exist");
  }
  hid_t space, dtype;
  space = H5Dget_space(data);
  dtype = H5Dget_type(data);
  result = readData(data, space, dtype, H5Dread);
  free(filename);
  free(dataname);
  H5Sclose(space);
  H5Tclose(dtype);
  H5Fclose(file);
  H5Dclose(data);
  return result;
}

// read data from an attribute
EXP K hdf5readAttrDataset(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K result;
  hid_t file, data, attr;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    return krr((S)"file does not exist");
  }
  dataname = kdbGetString(dname);
  data = H5Oopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset/group does not exist");
  }
  attrname = kdbGetString(aname);
  attr = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    free(filename);
    free(dataname);
    free(attrname);
    H5Oclose(data);
    H5Fclose(file);
    return krr((S)"attribute does not exist");
  }
  hid_t space, dtype;
  space = H5Aget_space(attr);
  dtype = H5Aget_type(attr);
  result = readData(attr, space, dtype, kdbH5Aread);
  free(filename);
  free(dataname);
  free(attrname);
  H5Sclose(space);
  H5Tclose(dtype);
  H5Oclose(data);
  H5Aclose(attr);
  H5Fclose(file);
  return result;
}

// define read utils

K readData(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc){
  return (H5T_COMPOUND == H5Tget_class(dtype)) ?
    readDataCompound(dset, space, dtype, readfunc):
    readDataSimple  (dset, space, dtype, readfunc);
}

K readDataSimple(hid_t dset, hid_t space, hid_t dtype, readfunc_t readfunc){
  K result;
  hid_t ntype;
  hsize_t npoints;
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  npoints = H5Sget_simple_extent_npoints(space);
  result = ktn(h2kType(ntype), npoints);
  readfunc(dset, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, kG(result));
  H5Tclose(ntype);
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
  keys = ktn(KS,0);
  vals = knk(0);
  for(i = 0; i < nmembers; ++i){
    mname = H5Tget_member_name(ntype, i);
    js(&keys, ss(mname));
    mclass = H5Tget_member_class(ntype, i);
    mtype  = H5Tget_member_type(dtype, i);
    mval = (mclass == H5T_STRING) ?
      readDataCompoundString(dset, mname, dims[0], mtype, space, readfunc):
      readDataCompoundValue (dset, mname, dims[0], mtype, space, readfunc);
    jk(&vals, mval);
    H5free_memory(mname);
  }
  free(dims);
  H5Tclose(ntype);
  H5Tclose(mtype);
  return(xT(xD(keys,vals)));
}

K readDataCompoundValue(hid_t dset, char* mname, hsize_t npoints, hid_t mtype, hid_t UNUSED(space), readfunc_t readfunc){
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
  result = knk(0);
  // variable length string
  if(H5Tis_variable_str(mtype)){
    H5Tset_size(stype, H5T_VARIABLE);
    memtype = H5Tcreate(H5T_COMPOUND, sizeof(char *));
    H5Tinsert(memtype, mname, 0, stype);
    vdata = (char **)calloc(npoints, sizeof(char *));
    readfunc(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, vdata);
    for(i = 0; i < npoints; ++i)
      jk(&result, kp(vdata[i]));
    free(vdata);
    H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, vdata);
  }
  // fixed length string
  else{
    size_t sz = H5Tget_size(mtype) + 1;
    H5Tset_size(stype, sz);
    memtype = H5Tcreate(H5T_COMPOUND, sz);
    H5Tinsert(memtype, mname, 0, stype);
    fdata = (char *)calloc(npoints * sz, sizeof(char));
    readfunc(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, fdata);
    for(i = 1; i < npoints; ++i)
      jk(&result, kp(fdata + i * sz));
    free(fdata);
  }
  H5Tclose(stype);
  H5Tclose(memtype);
  return result;
}
