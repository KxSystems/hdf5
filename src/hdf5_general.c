#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// initialize hdf5-kdb library
EXP K hdf5init(K UNUSED(dummy)){
  H5Eset_auto1(NULL,NULL); // disable errors from hdf5 side
  return KNL;
}

EXP K hdf5version(K UNUSED(x)){
  unsigned int maj, min, rel;
  if(H5get_libversion(&maj, &min, &rel) < 0)
    return krr((S)"error evaluating hdf5 version");
  else
    return kdbCreateDict("Major", kj(maj), "Minor", kj(min), "Release", kj(rel));
}

EXP K hdf5gc(K UNUSED(x)){
  return ki(H5garbage_collect());
}

K getShape(hid_t space){
  hsize_t *dims;
  int rank = H5Sget_simple_extent_ndims(space);
  K kdims = ktn(KJ, rank);
  dims = calloc(rank, sizeof(hsize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(int i = 0; i < rank; i++)
    kJ(kdims)[i] = dims[i];
  free(dims);
  return(kdims);
}

EXP K hdf5fileSize(K fname){
  if(!kdbCheckType("[Cs]", fname))
    return KNL;
  hsize_t fsize;
  hid_t file;
  char *filename;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  H5Fget_filesize(file, &fsize);
  H5Fclose(file);
  return(kf(fsize / 1000000.0));
}

EXP K hdf5dataSize(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  hsize_t dsize;
  hid_t file, data;
  char *filename, *dataname;
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
  dsize = H5Dget_storage_size(data);
  H5Dclose(data);
  return(kf(dsize / 1000000.0));
}

EXP K hdf5getDataShape(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K kdims;
  hid_t file, data, space;
  char *filename, *dataname;
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
  space = H5Dget_space(data);
  H5Dclose(data);
  kdims = getShape(space);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getDataPoints(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  J npoints;
  hid_t file, data, space;
  char *filename, *dataname;
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
    return krr((S)"error opening dataset/group");
  space  = H5Dget_space(data);
  H5Dclose(data);
  npoints = H5Sget_simple_extent_npoints(space);
  H5Sclose(space);
  return kj(npoints);
}

EXP K hdf5getAttrShape(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K kdims;
  hid_t file, data, attr, space;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  dataname = kdbGetString(dname);
  data  = H5Oopen(file, dataname, H5P_DEFAULT);
  free(dataname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  attrname = kdbGetString(aname);
  attr  = H5Aopen(data, attrname, H5P_DEFAULT);
  free(attrname);
  H5Oclose(data);
  if(attr < 0)
    return krr((S)"error opening attribute");
  space = H5Aget_space(attr);
  H5Aclose(attr);
  kdims = getShape(space);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getAttrPoints(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data, attr, space;
  J npoints;
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
  attr   = H5Aopen(data, attrname, H5P_DEFAULT);
  free(attrname);
  H5Oclose(data);
  if(attr < 0)
    return krr((S)"error opening attribute");
  space  = H5Aget_space(attr);
  H5Aclose(attr);
  npoints = H5Sget_simple_extent_npoints(space);
  H5Sclose(space);
  return kj(npoints);
}

// is file a hdf5 file
EXP K hdf5ishdf5(K fname){
  if(!kdbCheckType("[Cs]", fname))
    return KNL;
  htri_t filechk;
  char *filename = kdbGetString(fname);
  filechk = H5Fis_hdf5(filename);
  free(filename);
  return kb(filechk > 0 ? 1 : 0);
}

// does requested object (group/dataset) exist
EXP K hdf5isObject(K fname, K oname){
  if(!kdbCheckType("[Cs][Cs]", fname, oname))
    return KNL;
  htri_t oexists;
  hid_t file;
  char *filename, *objname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  objname = kdbGetString(oname);
  oexists = H5Oexists_by_name(file, objname, H5P_DEFAULT);
  free(objname);
  return kb(oexists > 0 ? 1 : 0);
}

// does requested attribute exist
EXP K hdf5isAttr(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  htri_t aexists;
  hid_t file, data;
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
  aexists = H5Aexists(data, attrname);
  free(attrname);
  H5Oclose(data);
  return kb(aexists > 0 ? 1 : 0);
}

EXP K hdf5datasetType(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K ktype;
  hid_t file, data, dtype, ntype;
  char *filename, *dataname;
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
  dtype = H5Dget_type(data);
  H5Dclose(data);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  H5Tclose(dtype);
         if(H5Tequal(ntype, H5T_NATIVE_CHAR))
    ktype = ks((S)"char");
  else if(H5Tequal(ntype, H5T_NATIVE_SHORT))
    ktype = ks((S)"short");
  else if(H5Tequal(ntype, H5T_NATIVE_INT))
    ktype = ks((S)"int");
  else if(H5Tequal(ntype, H5T_NATIVE_LONG))
    ktype = ks((S)"long");
  else if(H5Tequal(ntype, H5T_NATIVE_LLONG))
    ktype = ks((S)"llong");
  else if(H5Tequal(ntype, H5T_NATIVE_UCHAR))
    ktype = ks((S)"uchar");
  else if(H5Tequal(ntype, H5T_NATIVE_USHORT))
    ktype = ks((S)"ushort");
  else if(H5Tequal(ntype, H5T_NATIVE_UINT))
    ktype = ks((S)"uint");
  else if(H5Tequal(ntype, H5T_NATIVE_ULONG))
    ktype = ks((S)"ulong");
  else if(H5Tequal(ntype, H5T_NATIVE_ULLONG))
    ktype = ks((S)"ullong");
  else if(H5Tequal(ntype, H5T_NATIVE_FLOAT))
    ktype = ks((S)"float");
  else if(H5Tequal(ntype, H5T_NATIVE_DOUBLE))
    ktype = ks((S)"double");
  else if(H5Tequal(ntype, H5T_NATIVE_B8))
    ktype = ks((S)"b8");
  else if(H5Tequal(ntype, H5T_NATIVE_B16))
    ktype = ks((S)"b16");
  else if(H5Tequal(ntype, H5T_NATIVE_B32))
    ktype = ks((S)"b32");
  else if(H5Tequal(ntype, H5T_NATIVE_B64))
    ktype = ks((S)"b64");
  else
    ktype = ks((S)"");
  H5Tclose(ntype);
  return ktype;
}

// return type, ndims, dims
EXP K hdf5datasetInfo(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K ktype, kdims, kndims;
  ktype  = hdf5datasetType(fname, dname);
  kdims  = hdf5getDataShape(fname, dname);
  kndims = ki(kdims->n);
  return kdbCreateDict("type", ktype, "ndims", kndims, "dims", kdims);
}

EXP K hdf5copyObject(K srcfile, K src_obj, K dstfile, K dst_obj){
  if(!kdbCheckType("[Cs][Cs][Cs][Cs]", srcfile, src_obj, dstfile, dst_obj))
    return KNL;
  hid_t src, dst, status;
  char *srcfilename, *dstfilename, *src_objname, *dst_objname;
  srcfilename = kdbGetString(srcfile);
  src = H5Fopen(srcfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(srcfilename);
  if(src < 0)
    return krr((S)"error opening source file");
  dstfilename = kdbGetString(dstfile);
  dst = H5Fopen(dstfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(dstfilename);
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
