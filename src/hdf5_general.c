#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// Retrieve the shape of a dataspace
K hdf5getShape(hid_t space){
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

EXP K hdf5getDataShape(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K kdims;
  hid_t file, data, space;
  char *filename, *dataname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
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
  space = H5Dget_space(data);
  kdims = hdf5getShape(space);
  // clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getDataPoints(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  J points;
  hid_t file, data, space;
  char *filename, *dataname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  dataname = kdbGetString(dname);
  data = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  space  = H5Dget_space(data);
  points = H5Sget_simple_extent_npoints(space); // number of datapoints
  // Clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  H5Sclose(space);
  return kj(points);
}

EXP K hdf5getAttrShape(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K kdims;
  hid_t file, data, attr, space;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    return krr((S)"file does not exist");
  }
  dataname = kdbGetString(dname);
  data  = H5Oopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  attrname = kdbGetString(aname);
  attr  = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    free(attrname);
    free(dataname);
    free(filename);
    H5Oclose(data);
    H5Fclose(file);
    return krr((S)"attribute could not be opened");
  }
  space = H5Aget_space(attr);
  kdims = hdf5getShape(space);
  free(filename);
  free(dataname);
  free(attrname);
  H5Oclose(data);
  H5Aclose(attr);
  H5Fclose(file);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getAttrPoints(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data, attr, space;
  J points;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
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
    return krr((S)"dataset/group could not be opened");
  }
  attrname = kdbGetString(aname);
  attr   = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    free(attrname);
    free(dataname);
    free(filename);
    H5Oclose(data);
    H5Fclose(file);
    return krr((S)"attribute could not be opened");
  }
  space  = H5Aget_space(attr);
  points = H5Sget_simple_extent_npoints(space);
  free(filename); 
  free(dataname);
  free(attrname);
  H5Oclose(data);
  H5Fclose(file);
  H5Aclose(attr);
  H5Sclose(space);
  return kj(points);
}

// Is the file being passed a hdf5 file
EXP K hdf5ishdf5(K fname){
  if(!kdbCheckType("[Cs]", fname))
    return KNL;
  htri_t filechk;
  char *filename = kdbGetString(fname);
  filechk = H5Fis_hdf5(filename);
  free(filename);
  return (filechk > 0) ? kb(1) : kb(0);
}

// Does the requested attribute exist
EXP K hdf5isAttr(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data;
  htri_t aexists;
  char *filename = kdbGetString(fname);
  // Open file and dataset, check if a named attribute exists
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = kdbGetString(dname);
  data = H5Oopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Oclose(data);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  char *attrname = kdbGetString(aname);
  aexists = H5Aexists(data, attrname);
  // Clean up
  free(filename);
  free(dataname);
  free(attrname);
  H5Oclose(data);
  H5Fclose(file);
  // Attribute does not exist
  if(aexists <= 0)
    return kb(0);
  else
    return kb(1);
}

// Does the requested object (group/dataset exist)
/* oname = object name */ 
EXP K hdf5isObject(K fname, K oname){
  if(!kdbCheckType("[Cs][Cs]", fname, oname))
    return KNL;
  hid_t file;
  htri_t isobj;
  char *filename = kdbGetString(fname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *objname  = kdbGetString(oname);
  isobj = H5Oexists_by_name(file,objname, H5P_DEFAULT);
  free(filename);
  free(objname);
  H5Fclose(file);
  // Attribute does not exist
  if(isobj <= 0)
    return kb(0);
  else
    return kb(1);
}

// Return the major/minor and release versions as a dictionary
EXP K hdf5version(K UNUSED(x)){
  unsigned int maj, min, rel;
  if(H5get_libversion(&maj, &min, &rel)<0)
    return krr((S)"Error evaluating version of HDF5 C api");
  else
    return kdbCreateDict("Major", kj(maj), "Minor", kj(min), "Release", kj(rel));
}

// Garbage collection for hdf5 interface, return the data collected from free lists
EXP K hdf5gc(K UNUSED(x)){
  return ki(H5garbage_collect());
}

// Return the rank, type, and dimensionality of the dataset
EXP K hdf5datasetInfo(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  K kdims, kdtype;
  hid_t file, data, dtype, ntype, space;
  char *filename = kdbGetString(fname);
  int rank;
  // Open files and dataset and retrieve the dataspace
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = kdbGetString(dname);
  data  = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Dclose(data);
    H5Fclose(file);
    return krr((S)"dataset could not be opened");
  }
  space = H5Dget_space(data);
  // Retrieve dimensionality information
  rank = H5Sget_simple_extent_ndims(space);
  hsize_t dims[rank];
  H5Sget_simple_extent_dims(space, dims, NULL);
  kdims = ktn(KI,rank);
  for(int i=0;i<rank;i++)
    kI(kdims)[i] = dims[i];
  // Retrieve the type of the dataset 
  dtype = H5Dget_type(data);
  ntype = H5Tget_native_type(dtype,H5T_DIR_ASCEND);
  // Assign the correct k-symbol as return
  if(H5Tequal(ntype, HDF5INT))
    kdtype = ks((S)"integer");
  else if(H5Tequal(ntype, HDF5LONG))
    kdtype = ks((S)"long");
  else if(H5Tequal(ntype, HDF5FLOAT))
    kdtype = ks((S)"float");
  else if(H5Tequal(ntype, HDF5REAL))
    kdtype = ks((S)"real");
  else if(H5Tequal(ntype, HDF5SHORT))
    kdtype = ks((S)"short");
  else 
    kdtype = ks((S)"char");
  // Clean up
  free(filename);
  free(dataname);
  H5Dclose(data);
  H5Fclose(file);
  H5Tclose(dtype);
  H5Tclose(ntype);
  return kdbCreateDict("type", kdtype, "ndims", ki(rank), "dims", kdims);
}

// Copy the contents of one object to another location within a file or in a completely different file
/* oname = object name
 * fdest = destination folder
 * odest = destination object
*/
EXP K hdf5copyObject(K fname, K oname, K fdest, K odest){
  if(!kdbCheckType("[Cs][Cs][Cs][Cs]", fname, oname, fdest, odest))
    return KNL;
  char *filename = kdbGetString(fname);
  char *destname = kdbGetString(fdest);
  K res;
  hid_t file, dest;
  // Open files (can be the same) and copy object reference from one point to another
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  dest = H5Fopen(destname, H5F_ACC_RDWR, H5P_DEFAULT);
  if((file < 0) || dest < 0){
    free(filename);
    free(destname);
    H5Fclose(file);
    H5Fclose(dest);
    return krr((S)"Opening of files unsuccessful");
  }
  char *objname  = kdbGetString(oname);
  char *destobj  = kdbGetString(odest);
  if(H5Ocopy(file, objname, dest, destobj, H5P_DEFAULT, H5P_DEFAULT)<0)
    res = krr((S)"Error while copying the object");
  else
    res = 0;
  // Clean up
  free(filename);
  free(objname);
  free(destname);
  free(destobj);
  H5Fclose(file);
  H5Fclose(dest);
  return res;
}

// Get the size of a hdf5 file in megabytes
EXP K hdf5fileSize(K fname){
  if(!kdbCheckType("[Cs]", fname))
    return KNL;
  char *filename = kdbGetString(fname);
  double megab;
  hid_t file;
  hsize_t fsize;
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  H5Fget_filesize(file, &fsize);
  megab = (double)fsize / 1000000; 
  free(filename);
  H5Fclose(file);
  return(kf(megab));
}

EXP K hdf5dataSize(K fname, K dname){
  if(!kdbCheckType("[Cs][Cs]", fname, dname))
    return KNL;
  char *filename = kdbGetString(fname);
  char *dataname = kdbGetString(dname);
  hid_t file, data;
  hsize_t dsize;
  double megab;
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file); 
    return krr((S)"file does not exist");
  }
  data = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Dclose(data);
    H5Fclose(file);
    return krr((S)"dataset could not be opened");
  }
  dsize = H5Dget_storage_size(data);
  megab = (double)dsize / 1000000;
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  return(kf(megab));
}
