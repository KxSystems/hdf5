#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// declare create utils
K createNumeric(hid_t loc, char *name, K kdims, K ktype, createfunc_t create, closefunc_t close);
K createString(hid_t loc, char *name, K kdims, createfunc_t create, closefunc_t close);

EXP K hdf5createFile(K fname){
  if(!kdbCheckType("C", fname))
    return KNL;
  hid_t file;
  char *filename;
  filename = kdbGetString(fname);
  file = H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error creating file");
  H5Fclose(file);
  return KNL;
}

EXP K hdf5createGroup(K fname, K gname){
  if(!kdbCheckType("CC", fname, gname))
    return KNL;
  hid_t file, group;
  hid_t gcpl; // group creation property list
  char *filename, *groupnames;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  groupnames = kdbGetString(gname);
  gcpl = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(gcpl, 1); // create intermediate groups
  group = H5Gcreate(file, groupnames, gcpl, H5P_DEFAULT, H5P_DEFAULT);
  free(groupnames);
  H5Pclose(gcpl);
  H5Fclose(file);
  if(group < 0)
    return krr((S)"error creating group");
  H5Gclose(group);
  return KNL;
}

EXP K hdf5createDataset(K fname, K dname, K kdims, K ktype){
  if(!kdbCheckType("CCJc", fname, dname, kdims, ktype))
    return KNL;
  hid_t file;
  ktypegroup_t dtype;
  char *filename, *dataname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  dataname = kdbGetString(dname);
  dtype = getKTypeGroup(ktype->g);
  if(dtype == NUMERIC)
    createNumeric(file, dataname, kdims, ktype, H5Dcreate, H5Dclose);
  else if(dtype == STRING)
    createString(file, dataname, kdims, H5Dcreate, H5Dclose);
  else
    krr((S)"unsupported datatype");
  free(dataname);
  H5Fclose(file);
  return KNL;
}

EXP K hdf5createAttr(K fname, K dname, K aname, K kdims, K ktype){
  if(!kdbCheckType("CCCJc", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data;
  ktypegroup_t dtype;
  char *filename, *dataname, *attrname;
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
  dtype = getKTypeGroup(ktype->g);
  if(dtype == NUMERIC)
    createNumeric(data, attrname, kdims, ktype, kdbH5Acreate, H5Aclose);
  else if(dtype == STRING)
    createString(data, attrname, kdims, kdbH5Acreate, H5Aclose);
  else
    krr((S)"unsupported datatype");
  free(attrname);
  H5Oclose(data);
  return KNL;
}

// define create utils

K createNumeric(hid_t loc, char *name, K kdims, K ktype, createfunc_t create, closefunc_t close){
  hid_t space, obj, dtype;
  hsize_t dims[3];
  int rank, i;
  rank = kdims->n;
  if(rank > 3)
    return krr((S)"numerical datasets must have dimensionality <= 3");
  for(i = 0; i < rank; ++i)
    dims[i] = kJ(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  if(space < 0)
    return krr((S)"error creating dataspace");
  dtype = k2hType(ktype->g);
  obj = create(loc, name, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  if(obj < 0)
    return krr((S)"error creating dataset");
  close(obj);
  return KNL;
}

K createString(hid_t loc, char *name, K kdims, createfunc_t create, closefunc_t close){
  hid_t space, obj;
  hsize_t dims[1];
  int rank;
  rank = kdims->n;
  if(rank != 1)
    return krr((S)"string datasets must have dimensionality 1");
  dims[0] = kJ(kdims)[0];
  space = H5Screate_simple(1, dims, NULL);
  if(space < 0)
    return krr((S)"error creating dataspace");
  obj = create(loc, name, varstringtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  if(obj < 0)
    return krr((S)"error creating dataset");
  close(obj);
  return KNL;
}
