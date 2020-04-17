/* 
 * The following script contains utilities for the read functionality
 * relating both to the compound and individual datatypes
 * the reasoning for this is to remove clutter in the executable functions
 */

/* Read logic for unsigned numeric types */

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

K readBitfield(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i, rank;
  long points;
  unsigned char *rdata;
  if(strcmp(rdtyp,"a")==0)
    space = H5Aget_space(dset);
  else
    space = H5Dget_space(dset);
  points = H5Sget_simple_extent_npoints(space);
  rdata = (unsigned char *)malloc(points * sizeof (unsigned char));

  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, H5T_NATIVE_B8, rdata);
  else
    H5Dread(dset, H5T_NATIVE_B8, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  z = ktn(KG,points);
  for (i=0;i<points;i++)
    kG(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readByte(hid_t dset,char *rdtyp){
  hid_t space;
  K z;
  int i, rank;
  long points;
  unsigned char *rdata;
  if(strcmp(rdtyp,"a")==0)
    space = H5Aget_space(dset);
  else
    space = H5Dget_space(dset);
  points = H5Sget_simple_extent_npoints(space);
  rdata = (unsigned char *)malloc(points * sizeof (unsigned char));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, HDF5BYTE, rdata);
  else
    H5Dread(dset, HDF5BYTE, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  z = ktn(KG,points);
  for (i=0;i<points;i++)
    kG(z)[i] = *(rdata + i);
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

/* Read logic for fixed and variable length character string */

K readChar(hid_t dset, char *rdtyp){
  hid_t space, memtype,dtype;
  K vla;
  hsize_t dims[1];
  char **rdata;
  // memory type is standard C string
  memtype = H5Tcopy(H5T_C_S1);

  if(strcmp("d",rdtyp)==0){
    dtype = H5Dget_type(dset);
    space = H5Dget_space(dset);
  }
  else{
    dtype = H5Aget_type(dset);
    space = H5Aget_space(dset);
  }
  // Assign memory for char arrays
  H5Sget_simple_extent_dims(space, dims, NULL);
  rdata = (char **)malloc(dims[0] * sizeof(char *));

  if(H5Tis_variable_str(dtype)){
    H5Tset_size(memtype, H5T_VARIABLE);
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, memtype, rdata);
    else
      H5Dread(dset,memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  else{
    size_t sz = H5Tget_size(dtype);
    sz++;
    H5Tset_size(memtype,sz);
    rdata[0] = (char *)malloc(dims[0] * sz * sizeof(char));
    for (int j=1; j<dims[0]; j++)
      rdata[j] = rdata[0] + j * sz;
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, memtype, rdata[0]);
    else
      H5Dread(dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata[0]);
  }
  // Write the read data to a K object either a char array or list
  vla = knk(0);
  for (int i=0; i<dims[0]; i++)
    jk(&vla, kp((char*)rdata[i]));
  // Clean up
  // Reclaim unused memory from variable length data
  if(H5Tis_variable_str(dtype))
    H5Dvlen_reclaim(memtype, space, H5P_DEFAULT, rdata);
  free(rdata);
  H5Sclose(space);
  H5Tclose(memtype);
  return vla;
}

/* Read logic for unsigned short/int/long types */

K readUShort(hid_t dset, char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  unsigned short *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned short*)malloc(points *sizeof(unsigned short));
    H5Aread(dset, H5T_NATIVE_USHORT, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned short*)malloc(points *sizeof(unsigned short));
    H5Dread(dset, H5T_NATIVE_USHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KI,points);
  for (i=0;i<points;i++)
    kI(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readUInt(hid_t dset, char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  unsigned int *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned int*)malloc(points *sizeof(unsigned int));
    H5Aread(dset, H5T_NATIVE_UINT, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned int*)malloc(points *sizeof(unsigned int));
    H5Dread(dset, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KJ,points);
  for (i=0;i<points;i++)
    kJ(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

K readULong(hid_t dset, char *rdtyp){
  hid_t space;
  K z;
  int i;
  long points;
  unsigned long *rdata;
  if(strcmp(rdtyp,"a")==0){
    space = H5Aget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned long*)malloc(points *sizeof(unsigned long));
    H5Aread(dset, H5T_NATIVE_ULONG, rdata);
  }
  else{
    space = H5Dget_space(dset);
    points = H5Sget_simple_extent_npoints(space);
    rdata = (unsigned long*)malloc(points *sizeof(unsigned long));
    H5Dread(dset, H5T_NATIVE_ULONG, H5S_ALL, H5S_ALL, H5P_DEFAULT,rdata);
  }
  z = ktn(KJ,points);
  for (i=0;i<points;i++)
    kJ(z)[i] = *(rdata + i);
  free(rdata);
  H5Sclose(space);
  return(z);
}

/* Logic for reading individual compound types from a compound dataset */

K compoundInteger(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type,char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  if(H5Tequal(item_type,HDF5INT)){
    int *rdata;
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(int));
    H5Tinsert(cmpd_dset, memb_name, 0, item_type);
    rdata = (int*)malloc(dims * sizeof(int));
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    array = ktn(KI, dims);
    for(int j = 0; j < dims; j++)
      kI(array)[j] = *(rdata + j);
    free(rdata);
  }
  else if(H5Tequal(item_type,HDF5LONG)){
    long *rdata;
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(long));
    H5Tinsert(cmpd_dset, memb_name, 0, item_type);
    rdata = (long*)malloc(dims * sizeof(long));
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    array = ktn(KJ, dims);
    for(int j = 0; j < dims; j++)
      kJ(array)[j] = *(rdata + j);
    free(rdata);
  }
  else if(H5Tequal(item_type,HDF5SHORT)){
    short *rdata;
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(short));
    H5Tinsert(cmpd_dset, memb_name, 0, item_type);
    rdata = (short*)malloc(dims * sizeof(short));
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    array = ktn(KH, dims);
    for(int j = 0; j < dims; j++)
      kH(array)[j] = *(rdata + j);
    free(rdata);
  }
  H5Tclose(cmpd_dset);
  return array;
} 

K compoundFloat(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type,char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  if(H5Tequal(item_type,HDF5FLOAT)){
    double *rdata;
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(double));
    H5Tinsert(cmpd_dset, memb_name, 0, item_type);
    rdata = (double*)malloc(dims * sizeof(double));
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    array = ktn(KF, dims);
    for(int j = 0; j < dims; j++)
      kF(array)[j] = *(rdata + j);
    free(rdata);
  }
  else if(H5Tequal(item_type,HDF5REAL)){
    float *rdata;
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(float));
    H5Tinsert(cmpd_dset, memb_name, 0, item_type);
    rdata = (float*)malloc(dims * sizeof(float));
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    array = ktn(KE, dims);
    for(int j = 0; j < dims; j++)
      kE(array)[j] = *(rdata + j);
    free(rdata);
  }
  H5Tclose(cmpd_dset);
  return array;
}

K compoundByte(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type,char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  unsigned char *rdata;
  cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(unsigned char));
  H5Tinsert(cmpd_dset, memb_name, 0, item_type);
  rdata = (unsigned char*)malloc(dims * sizeof(unsigned char));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, cmpd_dset, rdata);
  else
    H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  array = ktn(KG, dims);
  for(int j = 0; j < dims; j++)
    kG(array)[j] = *(rdata + j);
  free(rdata);
  H5Tclose(cmpd_dset);
  return array;
}

K compoundBool(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type, char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  hbool_t *rdata;
  cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(unsigned long));
  H5Tinsert(cmpd_dset, memb_name, 0, item_type);
  rdata = (hbool_t *)malloc(dims * sizeof(hbool_t));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, cmpd_dset, rdata);
  else
    H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  array = ktn(KG, dims);
  for(int j = 0; j < dims; j++)
    kG(array)[j] = *(rdata + j);
  free(rdata);
  H5Tclose(cmpd_dset);
  return array;
}

/* Read fixed or variable length strings from a compound dataset */

K compoundString(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type,char *rdtyp){
  hid_t cmpd_dset, temp_type;
  int j;
  K array = knk(0);
  char **rdata;
  rdata = (char **)malloc(dims * sizeof(char *));
  temp_type = H5Tcopy(H5T_C_S1);
  if(H5Tis_variable_str(item_type)){
    H5Tset_size(temp_type, H5T_VARIABLE);
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(char *));
    H5Tinsert(cmpd_dset, memb_name, 0, temp_type);
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset, cmpd_dset, rdata);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  }
  else{
    size_t sz = H5Tget_size(item_type);
    sz++;
    H5Tset_size(temp_type, sz);
    cmpd_dset = H5Tcreate(H5T_COMPOUND, sz);
    H5Tinsert(cmpd_dset, memb_name, 0, temp_type);
    rdata[0] = (char *)malloc(dims * sz * sizeof(char));
    for (int j=1; j<dims; j++)
      rdata[j] = rdata[0] + j * sz;
    if(strcmp(rdtyp,"a")==0)
      H5Aread(dset,cmpd_dset,rdata[0]);
    else
      H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata[0]);
  }
  for(int j=0; j<dims; j++)
    jk(&array,kp((char*)rdata[j]));
  free(rdata);
  H5Tclose(temp_type);
  H5Tclose(cmpd_dset);
  return array;
}

/* Read unsigned datatypes from compound datasets */

K compoundUInt(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type, char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  unsigned *rdata;
  cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(unsigned));
  H5Tinsert(cmpd_dset, memb_name, 0, item_type);
  rdata = (unsigned *)malloc(dims * sizeof(unsigned));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, cmpd_dset, rdata);
  else
    H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  array = ktn(KJ, dims);
  for(int j = 0; j < dims; j++){
    kJ(array)[j] = *(rdata + j);
  }
  free(rdata);
  H5Tclose(cmpd_dset);
  return array;
}

K compoundUShort(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type, char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  unsigned short *rdata;
  cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(unsigned short));
  H5Tinsert(cmpd_dset, memb_name, 0, item_type);
  rdata = (unsigned short*)malloc(dims * sizeof(unsigned short));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, cmpd_dset, rdata);
  else
    H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  array = ktn(KI, dims);
  for(int j = 0; j < dims; j++)
    kI(array)[j] = *(rdata + j);
  free(rdata);
  H5Tclose(cmpd_dset);
  return array;
}


K compoundULong(hid_t dset, char* memb_name, hsize_t dims, hid_t item_type, char *rdtyp){
  hid_t cmpd_dset;
  int j;
  K array;
  unsigned long *rdata;
  cmpd_dset = H5Tcreate(H5T_COMPOUND, sizeof(unsigned long));
  H5Tinsert(cmpd_dset, memb_name, 0, item_type);
  rdata = (unsigned long*)malloc(dims * sizeof(unsigned long));
  if(strcmp(rdtyp,"a")==0)
    H5Aread(dset, cmpd_dset, rdata);
  else
    H5Dread(dset, cmpd_dset, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  array = ktn(KJ, dims);
  for(int j = 0; j < dims; j++)
    kJ(array)[j] = *(rdata + j);
  free(rdata);
  H5Tclose(cmpd_dset);
  return array;
}
