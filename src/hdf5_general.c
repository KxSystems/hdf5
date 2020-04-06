/* --- General use functions --- 
 * The functions contained in this file are related to the evaluation of
 * information about datasets/attributes, garbage collection etc. These functions in 
 * themselves do not relate to the other files within this code base explicitly.
*/

// Retrieve the shape of a dataspace
EXP K hdf5getShape(hid_t space){
  // Assign appropriate elements
  K kdims;
  int rank,i;
  // Get # dimensions in the dataset and assign a space to hold these
  rank = H5Sget_simple_extent_ndims(space);
  hsize_t dims[rank];
  // Assign the individual dimensions to assigned space
  H5Sget_simple_extent_dims(space, dims, NULL);
  // Convert dimensions to appropriate kdb+ data
  kdims = ktn(KJ,(J)rank);
  for(i=0;i<rank;i++)
    kJ(kdims)[i] = dims[i];
  return(kdims);
}

EXP K hdf5getDataShape(K fname, K dname){
  // Assign appropriate elements
  K kdims;
  hid_t file, data, space;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  // Check datatypes are relevant
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  // Open relevant files and datasets
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data  = H5Dopen(file, dataname, H5P_DEFAULT);
  space = H5Dget_space(data);
  kdims = hdf5getShape(space);
  // Clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  H5Sclose(space);
  return kdims;
}

EXP K hdf5getDataPoints(K fname, K dname){
  // Assign appropriate elements
  hid_t file, data, space;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  long points;
  // Check relevant datatypes
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  // Open relevant file, dataset and dataspace
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data = H5Dopen(file, dataname, H5P_DEFAULT);
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
  K kdims;
  hid_t file, data, attr, space;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  if(!checkType("[Cs][Cs][Cs]",fname,dname,aname))
    return KNL;
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data  = isGroupData(file, dataname);
  attr  = H5Aopen(data, attrname, H5P_DEFAULT);
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
  hid_t file, data, attr, space;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  long points;
  if(!checkType("[Cs][Cs][Cs]",fname,dname,aname))
    return KNL;
  file   = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data   = isGroupData(file, dataname);
  attr   = H5Aopen(data, attrname, H5P_DEFAULT);
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
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  // Open file and dataset, check if a named attribute exists
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data = isGroupData(file, dataname);
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
  if(!checkType("[Cs][Cs]", fname, oname))
    return KNL;
  hid_t file;
  htri_t isobj;
  char *filename = getkstring(fname);
  char *objname  = getkstring(oname);
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
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
  H5get_libversion(&maj, &min, &rel);
  return xd("Major",ki(maj),"Minor",ki(min),"Release",ki(rel));
}

// Garbage collection for hdf5 interface, return the data collected from free lists
EXP K hdf5gc(K UNUSED(x)){
  return ki(H5garbage_collect());
}

// Return the rank, type, and dimensionality of the dataset
EXP K hdf5datasetInfo(K fname, K dname){
  K kdims, kdtype;
  hid_t file, data, dtype, ntype, space;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  int rank, i;
  if(!checkType("[Cs][Cs]",fname,dname))
    return KNL;
  // Open files and dataset and retrieve the dataspace
  file  = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data  = H5Dopen(file, dataname, H5P_DEFAULT);
  space = H5Dget_space(data);
  // Retrieve dimensionality information
  rank = H5Sget_simple_extent_ndims(space);
  hsize_t dims[rank];
  H5Sget_simple_extent_dims(space, dims, NULL);
  kdims = ktn(KI,rank);
  for(i=0;i<rank;i++)
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
  char *filename = getkstring(fname);
  char *objname  = getkstring(oname);
  char *destname = getkstring(fdest);
  char *destobj  = getkstring(odest);
  hid_t file_id, dest_id;
  // Open files (can be the same) and copy object reference from one point to another
  file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  dest_id = H5Fopen(destname, H5F_ACC_RDWR, H5P_DEFAULT);
  H5Ocopy(file_id, objname, dest_id, destobj, H5P_DEFAULT, H5P_DEFAULT);
  // Clean up
  free(filename);
  free(objname);
  free(destname);
  free(destobj);
  H5Fclose(file_id);
  H5Fclose(dest_id);
  return KNL;
}

// Get the size of a hdf5 file in megabytes
EXP K hdf5fileSize(K fname){
  disable_err();
  char *filename = getkstring(fname);
  double megab;
  hid_t file_id;
  hsize_t fsize;
  file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  H5Fget_filesize(file_id, &fsize);
  megab = (double)fsize / 1000; 
  free(filename);
  H5Fclose(file_id);
  return(kf(megab));
}

EXP K hdf5dataSize(K fname, K dname){
  disable_err();
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  hid_t file_id, data_id;
  hsize_t dsize;
  double megab;
  file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data_id = H5Dopen(file_id , dataname, H5P_DEFAULT);
  dsize = H5Dget_storage_size(data_id);
  megab = (double)dsize / 1000;
  free(filename);
  free(dataname);
  H5Fclose(file_id);
  H5Dclose(data_id);
  return(kf(megab));
}
