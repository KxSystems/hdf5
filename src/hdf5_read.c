/* --- Read functionality
 * The following functions are used in conversion to q data from hdf5.
 * These functions assume that numeric data is being passed as a contiguous list "(raze/)data",
 * this may need to be reevaluated in order to improve efficiency.
*/


// Function prototypes for utility functions relating to reading of hdf5 data 
// these are defined later in this script
K readData(hid_t data, char *rdtyp);
K readFloat(hid_t dset, char *rdtyp);
K readInt(hid_t dset, char *rdtyp);
K readLong(hid_t dset, char *rdtyp);
K readReal(hid_t dset, char *rdtyp);
K readShort(hid_t dset, char *rdtyp);
K readChar(hid_t dset, char *rdtyp);

// Read data from an attribute associated with a group or dataset
EXP K hdf5readAttrDataset(K fname, K dname, K aname){
  disable_err();
  // Type check inputs
  if(!checkType("[Cs][Cs][Cs]", fname, dname,aname))
    return KNL;
  // Define required elements
  K result;
  hid_t file, data, attr;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  // Open file and dataset
  file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  // error if file doesn't exist
  if(file < 0){
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"file does not exist");
  }
  data = isGroupData(file, dataname);
  if(data < 0){
    free(filename);
    free(dataname);
    free(attrname);
    H5Fclose(file);
    return krr((S)"dataset/group from which attribute is to be read to does not exist");
  }
  attr = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    closeGroupData(file,dataname,data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"This attribute does not exist");
  }
  result = readData(attr, "a");
  closeGroupData(file, dataname, data);
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Aclose(attr);
  return result;
}

EXP K hdf5readDataset(K fname, K dname){
  disable_err();
  // Type check inputs
  if(!checkType("[Cs][Cs]", fname, dname))
    return KNL;
  // Define required elements
  K result;
  hid_t file, data;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  // Open file and dataset
  file   = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    free(dataname);
    return krr((S)"file does not exist");
  }
  data   = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset does not exist");
  }
  result = readData(data, "d");
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  return result;
}


// Read numerical data to kdb from a hdf5 object
K readData(hid_t data,char *rdtyp){
  hid_t dtype, ntype;
  K result;
  // Get type based on form of object being read
  if(strcmp("d",rdtyp)==0)
    dtype = H5Dget_type(data);
  else if(strcmp("a",rdtyp)==0)
    dtype = H5Aget_type(data);
  ntype = H5Tget_native_type(dtype,H5T_DIR_ASCEND);
  if(H5Tequal(ntype, HDF5INT))
    result = readInt(data,rdtyp);
  else if(H5Tequal(ntype, HDF5LONG))
    result = readLong(data,rdtyp);
  else if(H5Tequal(ntype, HDF5FLOAT))
    result = readFloat(data,rdtyp);
  else if(H5Tequal(ntype, HDF5REAL))
    result = readReal(data,rdtyp);
  else if(H5Tequal(ntype, HDF5SHORT))
    result = readShort(data,rdtyp);
  else 
    result = readChar(data, rdtyp);
  H5Tclose(dtype);
  H5Tclose(ntype);
  return result;
}

K readFloat(hid_t dset,char *rdtyp){
  // Assign elements used within the function
  hid_t space;
  K z;
  int i;
  long points;
  double *rdata;
  // Check if (a)ttribute or (d)ataset is being read
  if(strcmp(rdtyp,"a")==0){
    // Retrieve the attribute space
    space = H5Aget_space(dset);
    // Get number of datapoints which must be read
    points = H5Sget_simple_extent_npoints(space);
    // Assign required memory
    rdata = (double *)malloc(points *sizeof(double));
    // Read the attribute data to memory buffer
    H5Aread(dset, HDF5FLOAT, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (double *)malloc(points *sizeof(double));
    H5Dread(dset, HDF5FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  // Create K float array of length points
  z = ktn(KF,points);
  // Assign all data in buffer to K array
  for (i=0;i<points;i++)
    kF(z)[i] = *(rdata + i);
  // Clean up
  free(rdata);
  H5Sclose(space);
  // Return the kdb data as a continguous list
  return(z);
}

K readInt(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  int *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (int *)malloc(points *sizeof(int));
    H5Aread(dset, HDF5INT, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (int *)malloc(points *sizeof(int));
    H5Dread(dset, HDF5INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KI,points);
  for (i=0;i<points;i++)
    kI(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readLong(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  long *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (long *)malloc(points *sizeof(long));
    H5Aread(dset, HDF5LONG, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (long *)malloc(points *sizeof(long));
    H5Dread(dset, HDF5LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KJ,points);
  for (i=0;i<points;i++)
    kJ(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readReal(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  float *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (float *)malloc(points *sizeof(float));
    H5Aread(dset, HDF5REAL, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (float *)malloc(points *sizeof(float));
    H5Dread(dset, HDF5REAL, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KE,points);
  for (i=0;i<points;i++)
    kE(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readShort(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  short *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (short *)malloc(points *sizeof(short));
    H5Aread(dset, HDF5SHORT, rdata);
  } 
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (short *)malloc(points *sizeof(short));
    H5Dread(dset, HDF5SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KH,points);
  for (i=0;i<points;i++)
    kH(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

// Read character datasets 
K readChar(hid_t dset, char *rdtyp){
  hid_t space, memtype;
  K vla;
  int i;
  hsize_t dims[1];
  char **rdata;
  // memory type is standard C string
  memtype = H5Tcopy(H5T_C_S1);
  // data can be of variable length
  H5Tset_size(memtype, H5T_VARIABLE);
  // Deal with data associated with attribute
  if(strcmp(rdtyp,"a")==0){
    // get the dataspace and retrieve dimensionality
    space = H5Aget_space(dset);
    H5Sget_simple_extent_dims(space, dims, NULL);
    // Allocate memory to write to
    rdata = (char **)malloc(dims[0] * sizeof(char *));
    H5Aread(dset, memtype, rdata);
  }
  // Deal with data assoicated with dataset
  else{
    space = H5Dget_space(dset);
    H5Sget_simple_extent_dims(space, dims, NULL);
    rdata = (char **)malloc(dims[0] * sizeof(char *));
    H5Dread(dset,memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  // Write the read data to a K object either a char array or list
  if(1==dims[0])
   vla = kp(rdata[0]);
  else{
    vla = knk(0);
    for (i=0; i<dims[0]; i++)
      jk(&vla, kp(rdata[i]));
  }
  // Clean up
  // Reclaim unused memory from variable length data
  H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, rdata);
  free(rdata);
  H5Sclose(space);
  H5Tclose(memtype);
  return vla;
}
