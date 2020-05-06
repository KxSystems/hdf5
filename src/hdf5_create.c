#include <stdlib.h>
#include "hdf5_utils.h"

// declare create utils
K createData(hid_t loc, K name, K kdims, K ktype, createfunc_t create, closefunc_t close);

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
  char *groupnames;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
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
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  createData(file, dname, kdims, ktype, H5Dcreate, H5Dclose);
  H5Fclose(file);
  return KNL;
}

EXP K hdf5createAttr(K fname, K dname, K aname, K kdims, K ktype){
  if(!kdbCheckType("CCCJc", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Oopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  createData(data, aname, kdims, ktype, kdbH5Acreate, H5Aclose);
  H5Oclose(data);
  return KNL;
}

// define create utils
K createData(hid_t loc, K name, K kdims, K ktype, createfunc_t create, closefunc_t close){
  hid_t space, obj, dtype;
  char *namestr;
  ktypegroup_t dgroup;
  hsize_t dims[32];
  int rank, i;
  rank = kdims->n;
  if(rank > 32)
    return krr((S)"datasets must have dimensionality <= 32");
  for(i = 0; i < rank; ++i)
    dims[i] = kJ(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  if(space < 0)
    return krr((S)"error creating dataspace");
  dgroup = getKTypeGroup(ktype->g);
  if(dgroup == NUMERIC)
    dtype = k2hType(ktype->g);
  else if(dgroup == STRING)
    dtype = varstringtype;
  else{
    H5Sclose(space);
    return krr((S)"unsupported datatype");
  }
  namestr = kdbGetString(name);
  obj = create(loc, namestr, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  free(namestr);
  H5Sclose(space);
  if(obj < 0)
    return krr((S)"error creating dataset");
  close(obj);
  return KNL;
}
