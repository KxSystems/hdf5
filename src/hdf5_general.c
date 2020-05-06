#include <stdlib.h>
#include "hdf5_utils.h"

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy)){
  initvarstringtype();
  initerror();
  errorOff();
  return KNL;
}

// error handling
EXP K hdf5errorOn(K UNUSED(dummy)){
  errorOn();
  return KNL;
}
EXP K hdf5errorOff(K UNUSED(dummy)){
  errorOff();
  return KNL;
}

EXP K hdf5version(K UNUSED(dummy)){
  unsigned int maj, min, rel;
  if(H5get_libversion(&maj, &min, &rel) < 0)
    return krr((S)"error evaluating hdf5 version");
  else
    return kdbCreateDict("Major", kj(maj), "Minor", kj(min), "Release", kj(rel));
}

EXP K hdf5gc(K UNUSED(dummy)){
  return ki(H5garbage_collect());
}

K getShape(hid_t space){
  hsize_t rank, *dims, i;
  rank = H5Sget_simple_extent_ndims(space);
  K kdims = ktn(KJ, rank);
  dims = calloc(rank, sizeof(hsize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < rank; i++)
    kJ(kdims)[i] = dims[i];
  free(dims);
  return(kdims);
}

EXP K hdf5fileSize(K fname){
  if(!kdbCheckType("C", fname))
    return KNL;
  hsize_t fsize;
  hid_t file;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  H5Fget_filesize(file, &fsize);
  H5Fclose(file);
  return(kf(fsize / 1000000.0));
}

EXP K hdf5dataSize(K fname, K dname){
  if(!kdbCheckType("CC", fname, dname))
    return KNL;
  hsize_t dsize;
  hid_t file, data;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Dopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset");
  dsize = H5Dget_storage_size(data);
  H5Dclose(data);
  return(kf(dsize / 1000000.0));
}

EXP K hdf5getDataShape(K fname, K dname){
  if(!kdbCheckType("CC", fname, dname))
    return KNL;
  K kdims;
  hid_t file, data, space;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Dopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset");
  space = H5Dget_space(data);
  H5Dclose(data);
  if(space < 0)
    return krr((S)"error opening data space");
  kdims = getShape(space);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getAttrShape(K fname, K dname, K aname){
  if(!kdbCheckType("CCC", fname, dname, aname))
    return KNL;
  K kdims;
  hid_t file, data, attr, space;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
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
  H5Aclose(attr);
  if(space < 0)
    return krr((S)"error opening attribute space");
  kdims = getShape(space);
  H5Sclose(space);
  return kdims;
}

// is file a hdf5 file
EXP K hdf5ishdf5(K fname){
  if(!kdbCheckType("C", fname))
    return KNL;
  htri_t filechk;
  char *filename = kdbGetString(fname);
  filechk = H5Fis_hdf5(filename);
  free(filename);
  return kb(filechk > 0 ? 1 : 0);
}

// does requested object (group/dataset) exist
EXP K hdf5isObject(K fname, K oname){
  if(!kdbCheckType("CC", fname, oname))
    return KNL;
  htri_t oexists;
  hid_t file;
  char *objname;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  objname = kdbGetString(oname);
  oexists = H5Oexists_by_name(file, objname, H5P_DEFAULT);
  H5Fclose(file);
  free(objname);
  return kb(oexists > 0 ? 1 : 0);
}

// does requested attribute exist
EXP K hdf5isAttr(K fname, K dname, K aname){
  if(!kdbCheckType("CCC", fname, dname, aname))
    return KNL;
  htri_t aexists;
  hid_t file, data;
  char *attrname;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Oopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  attrname = kdbGetString(aname);
  aexists = H5Aexists(data, attrname);
  free(attrname);
  H5Oclose(data);
  return kb(aexists > 0 ? 1 : 0);
}

EXP K hdf5copyObject(K srcfile, K src_obj, K dstfile, K dst_obj){
  if(!kdbCheckType("CCCC", srcfile, src_obj, dstfile, dst_obj))
    return KNL;
  hid_t src, dst, status;
  char *src_objname, *dst_objname;
  src = kdbH5Fopen(srcfile, H5F_ACC_RDWR);
  if(src < 0)
    return krr((S)"error opening source file");
  dst = kdbH5Fopen(dstfile, H5F_ACC_RDWR);
  if(dst < 0){
    H5Fclose(src);
    return krr((S)"error opening destination file");
  }
  src_objname = kdbGetString(src_obj);
  dst_objname = kdbGetString(dst_obj);
  status = H5Ocopy(src, src_objname, dst, dst_objname, H5P_DEFAULT, H5P_DEFAULT);
  free(src_objname);
  free(dst_objname);
  H5Fclose(src);
  H5Fclose(dst);
  if(status < 0)
    return krr((S)"error copying object");
  return KNL;
}
