/* --- General use functions --- 
 * The functions contained in this file are related to the evaluation of
 * information about datasets/attributes, garbage collection etc. These functions in 
 * themselves do not relate to the other files within this code base explicitly.
*/

#include "hdf5.h"
#include "kdb_util.h"
#include "hdf5_utils.h"
#include <stdlib.h>

// Retrieve the shape of a dataspace
K hdf5getShape(hid_t space){
  // Assign appropriate elements
  K kdims;
  int rank;
  // Get # dimensions in the dataset and assign a space to hold these
  rank = H5Sget_simple_extent_ndims(space);
  hsize_t dims[rank];
  // Assign the individual dimensions to assigned space
  H5Sget_simple_extent_dims(space, dims, NULL);
  // Convert dimensions to appropriate kdb+ data
  kdims = ktn(KJ,(J)rank);
  for(int i=0;i<rank;i++)
    kJ(kdims)[i] = dims[i];
  return(kdims);
}

EXP K hdf5getDataShape(K fname, K dname){
  disable_err();
  // Check datatypes are relevant
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  // Assign appropriate elements
  K res;
  hid_t file, data, space;
  char *filename = getkstring(fname);
  // Open relevant files and datasets
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
  data  = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Dclose(data);
    H5Fclose(file);
    return krr((S)"dataset not appropriate for problem");
  }
  space = H5Dget_space(data);
  res = hdf5getShape(space);
  // Clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  H5Sclose(space);
  return res;
}

EXP K hdf5getDataPoints(K fname, K dname){
  disable_err();
  // Check relevant datatypes
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  // Assign appropriate elements
  hid_t file, data, space;
  char *filename = getkstring(fname);
  long points;
  // Open relevant file, dataset and dataspace
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
  data = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Dclose(data);
    H5Fclose(file);
    return krr((S)"dataset not appropriate for problem");
  }
  space = H5Dget_space(data);
  // Retrieve the number of datapoints in the dataset
  points = (J)H5Sget_simple_extent_npoints(space);
  // Clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  H5Sclose(space);
  return kj(points);
}

EXP K hdf5getAttrShape(K fname, K dname, K aname){
  disable_err();
  if(!checkType("[Cs][Cs][Cs]",fname,dname,aname))
    return KNL;
  K kdims;
  hid_t file, data, attr, space;
  char *filename = getkstring(fname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
  data  = isGroupData(file, dataname);
  if(data < 0){
    closeGroupData(file, dataname, data);
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  char *attrname = getkstring(aname);
  attr  = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    closeGroupData(file, dataname, data);
    free(attrname);
    free(dataname);
    free(filename);
    H5Fclose(file);
    H5Aclose(attr);
    return krr((S)"attribute could not be opened");
  }
  space = H5Aget_space(attr);
  kdims = hdf5getShape(space);
  closeGroupData(file, dataname, data);
  free(filename);
  free(dataname);
  free(attrname);
  H5Aclose(attr);
  H5Fclose(file);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getAttrPoints(K fname, K dname, K aname){
  disable_err();
  if(!checkType("[Cs][Cs][Cs]",fname,dname,aname))
    return KNL;
  hid_t file, data, attr, space;
  char *filename = getkstring(fname);
  long points;
  file   = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
  data   = isGroupData(file, dataname);
  if(data < 0){
    free(filename);
    free(dataname);
    closeGroupData(file, dataname, data);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  char *attrname = getkstring(aname);
  attr   = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    closeGroupData(file, dataname, data);
    free(attrname);
    free(dataname);
    free(filename);
    H5Fclose(file);
    H5Aclose(attr);
    return krr((S)"attribute could not be opened");
  }
  space  = H5Aget_space(attr);
  points = (J)H5Sget_simple_extent_npoints(space);
  closeGroupData(file, dataname, data);
  free(filename); 
  free(dataname);
  free(attrname);
  H5Fclose(file);
  H5Aclose(attr);
  H5Sclose(space);
  return kj(points);
}

// Is the file being passed a hdf5 file
EXP K hdf5ishdf5(K fname){
  disable_err();
  if(!checkType("[Cs]",fname))
    return KNL;
  htri_t file_nm;
  char *filename = getkstring(fname);
  // Check if the file is a hdf5 file
  file_nm = ish5(filename);
  free(filename);
  // File being passed is not a hdf5
  if(file_nm <= 0)
    return kb(0);
  // File exists and is hdf5
  else
    return kb(1);
}

// Does the requested attribute exist
EXP K hdf5isAttr(K fname, K dname, K aname){
  disable_err();
  if(!checkType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data;
  htri_t aexists;
  char *filename = getkstring(fname);
  // Open file and dataset, check if a named attribute exists
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
  data = isGroupData(file, dataname);
  if(data < 0){
    free(filename);
    free(dataname);
    closeGroupData(file, dataname, data);
    H5Fclose(file);
    return krr((S)"dataset/group could not be opened");
  }
  char *attrname = getkstring(aname);
  aexists = H5Aexists(data, attrname);
  // Clean up
  closeGroupData(file,dataname,data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  // Attribute does not exist
  if(aexists <= 0)
    return kb(0);
  else
    return kb(1);
}

// Does the requested object (group/dataset exist)
/* oname = object name */ 
EXP K hdf5isObject(K fname, K oname){
  disable_err();
  if(!checkType("[Cs][Cs]", fname, oname))
    return KNL;
  hid_t file;
  htri_t isobj;
  char *filename = getkstring(fname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *objname  = getkstring(oname);
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
  disable_err();
  unsigned int maj, min, rel;
  if(H5get_libversion(&maj, &min, &rel)<0)
    return krr((S)"Error evaluating version of HDF5 C api");
  else
    return xd("Major",kj(maj),"Minor",kj(min),"Release",kj(rel));
}

// Garbage collection for hdf5 interface, return the data collected from free lists
EXP K hdf5gc(K UNUSED(x)){
  disable_err();
  return ki(H5garbage_collect());
}

// Return the rank, type, and dimensionality of the dataset
EXP K hdf5datasetInfo(K fname, K dname){
  disable_err();
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  K kdims, kdtype;
  hid_t file, data, dtype, ntype, space;
  char *filename = getkstring(fname);
  int rank;
  // Open files and dataset and retrieve the dataspace
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *dataname = getkstring(dname);
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
  return xd("type",kdtype,"ndims",ki(rank),"dims",kdims);
}

// Copy the contents of one object to another location within a file or in a completely different file
/* oname = object name
 * fdest = destination folder
 * odest = destination object
*/
EXP K hdf5copyObject(K fname, K oname, K fdest, K odest){
  disable_err();
  if(!checkType("[Cs][Cs][Cs][Cs]",fname, oname, fdest, odest))
    return KNL;
  char *filename = getkstring(fname);
  char *destname = getkstring(fdest);
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
  char *objname  = getkstring(oname);
  char *destobj  = getkstring(odest);
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
  disable_err();
  if(!checkType("[Cs]",fname))
    return KNL;
  char *filename = getkstring(fname);
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
  disable_err();
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
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
