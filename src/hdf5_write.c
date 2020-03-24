/* --- Write functionality for attributes and datasets ---
 * The following function is used in writing kdb+ data to hdf5 format.
*/

// Function prototypes
static void writenumeric(K dset, hid_t data, char *data_attr);
int longwrite (hid_t h5data, K dset, long   *data, char *rdtype, long points, int idx);
int intwrite  (hid_t h5data, K dset, int    *data, char *rdtype, long points, int idx);
int shortwrite(hid_t h5data, K dset, short  *data, char *rdtype, long points, int idx);
int realwrite (hid_t h5data, K dset, float  *data, char *rdtype, long points, int idx);
int floatwrite(hid_t h5data, K dset, double *data, char *rdtype, long points, int idx);
static void writeChar(K dset, hid_t data, char *data_attr);

// Write data to an attribute
EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  disable_err();
  htri_t file_nm, aexists;
  int isdset;
  hid_t data, file,attr;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  char *ktyp = getkstring(ktype);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  // if file doesn't exist error out
  if(file < 0){
    free(filename);
    free(dataname);
    free(attrname);
    free(ktyp);
    return(krr((S)"file does not exist"));
  }
  data = isGroupData(file, dataname);
  // If dataset doesn't exist error out
  if(data < 0){
    free(filename);
    free(dataname);
    free(attrname);
    free(ktyp);
    H5Fclose(file);
    return(krr((S)"dataset or group you are attempting to write attribute to does not exist"));
  }
  // Check if type that is to be written to 
  if(1 == checkvalid(ktyp)){
    // Does the attribute already exist
    aexists = isattr(file, attrname);
    if(aexists == 0)
      createsimpleattr(data, attrname, kdims, ktype);
    // Could not evaluate if the attribute exists error out
    else if(aexists < 0){
      closeGroupData(file, dataname, data);
      free(attrname);
      free(dataname);
      free(filename);
      free(ktyp);
      return(krr((S)"check to ensure that attribute exists failed"));
    }
    attr = H5Aopen(data, attrname, H5P_DEFAULT);
    // write the kdb dset to the data file
    writenumeric(dset, attr, "a");
  }
  // Check if the type that is to be written to is character/symbol
  else if(2 == checkvalid(ktyp)){
    aexists = isattr(file, attrname);
    if(aexists == 0)
      createstrattr(data, attrname, kdims);
    else if(aexists < 0){
      closeGroupData(file, dataname, data);
      free(attrname);
      free(dataname);
      free(filename);
      free(ktyp);
      return(krr((S)"check to ensure that attribute exists failed"));
    }
    attr = H5Aopen(data, attrname, H5P_DEFAULT);
    writeChar(dset, attr, "a");
  }
  // error out if type is unsupported
  else{
    free(attrname);
    free(dataname);
    free(filename);
    free(ktyp);
    H5Fclose(file);
    return(krr((S)"Attempting to pass an unsupported datatype"));
  }
  // Clean up
  closeGroupData(file, dataname, data);
  free(attrname);
  free(dataname);
  free(filename);
  free(ktyp);
  H5Fclose(file);
  H5Aclose(attr);
  return 0;
}

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  disable_err();
  htri_t file_nm;
  int isdset;
  hid_t data, file;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *ktyp = getkstring(ktype);
  // Create a file is it does not exist
  file_nm = ish5(filename);
  if(file_nm < 0)
    createfile(filename);
  if(file_nm == 0){
    free(dataname);
    free(filename);
    free(ktyp);
    return krr((S)"This file already exists and is not a hdf5 file");
  }
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  // Create the appropriate dataset if it doesn't exist typed appropriately
  isdset = checkdataset(file, dataname);
  if(1 == checkvalid(ktyp)){
    if(isdset == 0)
      createsimpledataset(file, dataname, kdims, ktype);
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    // write the kdb dset to the data file
    writenumeric(dset, data, "d");
  }
  else if(2 == checkvalid(ktyp)){
    if(isdset == 0)
      createstrdataset(file, dataname, kdims);
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    writeChar(dset, data, "d");
  }
  else{
    free(dataname);
    free(filename);
    free(ktyp);
    H5Fclose(file);
    return(krr((S)"Attempting to pass an unsupported datatype"));
  }
  free(dataname);
  free(filename);
  free(ktyp);
  H5Dclose(data);
  H5Fclose(file);
  return 0;
}


static void writeFloat(K dset, hid_t data, char *rdtype){
  long i;
  hid_t space;
  // Allocate the memory needed for writing to hdf5
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else if(strcmp(rdtype,"a")==0)
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  double *dset_data = (double *)malloc(points *sizeof(double));
  floatwrite(data, dset, dset_data, rdtype, points, 0);
  // Clean up
  free(dset_data);
  H5Sclose(space);
}

static void writeInt(K dset, hid_t data, char *rdtype){
  long i;
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else if(strcmp(rdtype,"a")==0)
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  int *dset_data = (int *)malloc(points *sizeof(int));
  intwrite(data, dset, dset_data, rdtype, points, 0);
  free(dset_data);
  H5Sclose(space);
}

static void writeLong(K dset, hid_t data, char *rdtype){
  long i;
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else if(strcmp(rdtype,"a")==0)
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  long *dset_data = (long *)malloc(points *sizeof(long));
  longwrite(data, dset, dset_data, rdtype, points, 0);
  free(dset_data);
  H5Sclose(space);
}

static void writeReal(K dset, hid_t data, char *rdtype){
  hid_t space;
  long i;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else if(strcmp(rdtype,"a")==0)
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);  
  float *dset_data = (float *)malloc(points *sizeof(float));
  realwrite(data, dset, dset_data, rdtype, points,0);
  free(dset_data);
  H5Sclose(space);
}

static void writeShort(K dset, hid_t data, char *rdtype){
  hid_t space;
  long i;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else if(strcmp(rdtype,"a")==0)
    space = H5Aget_space(data);
  long  points = H5Sget_simple_extent_npoints(space);
  short *dset_data = (short *)malloc(points *sizeof(short));
  shortwrite(data, dset, dset_data, rdtype, points, 0);
  H5Sclose(space);
  free(dset_data);
}

static void writeChar(K dset, hid_t data, char *data_attr){
  long i,arr;
  hid_t memtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(memtype, H5T_VARIABLE);
  if((KC == dset->t) || -KC == dset->t)
    arr = 1;
  else
    arr = dset->n;
  char *wdata[arr];
  if((KC == dset->t) || -KC == dset->t)
   wdata[0] = getkstring(dset);
  else{
    for(i=0;i<dset -> n;i++)
      wdata[i] = getkstring(kK(dset)[i]);
  }
  if(strcmp(data_attr,"d")==0)
    H5Dwrite(data, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
  else if(strcmp(data_attr,"a")==0)
    H5Awrite(data, memtype, wdata);
  H5Tclose(memtype);
} 

static void writenumeric(K dset, hid_t data, char *data_attr){
  hid_t dtype,ntype;
  if(strcmp("d",data_attr)==0)
    dtype = H5Dget_type(data);
  else if(strcmp("a",data_attr)==0)
    dtype = H5Aget_type(data);
  ntype = H5Tget_native_type(dtype,H5T_DIR_ASCEND);
  if(H5Tequal(ntype, HDF5INT))
    writeInt(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5LONG))
    writeLong(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5FLOAT))
    writeFloat(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5REAL))
    writeReal(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5SHORT))
    writeShort(dset, data, data_attr);
  H5Tclose(dtype);
  H5Tclose(ntype);
}

int intwrite(hid_t h5data, K dset, int *data, char *rdtype, long points,int idx){
  int i,j;
  if(-KI == dset->t){
    *(data + idx + i) = dset->i;
    idx = idx + 1;
  }
  else if(KI == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kI(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = intwrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
          *(data + idx + j) = kI(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  // Write when the size of allocated buffer has been filled
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else if(strcmp(rdtype,"a")==0)
      H5Awrite(h5data, HDF5INT, data);
    return 0;
  }
  return idx;
}

int shortwrite(hid_t h5data, K dset, short *data, char *rdtype, long points,int idx){
  int i,j;
  if(-KH == dset->t){
    *(data + idx + i) = dset->h;
    idx = idx + 1;
  }
  else if(KH == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kH(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = shortwrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
           *(data + idx + j) = kH(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  // Write when the size of allocated buffer has been filled
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else if(strcmp(rdtype,"a")==0)
      H5Awrite(h5data, HDF5SHORT, data);
    return 0;
  }
  return idx;
}

int longwrite(hid_t h5data, K dset, long *data, char *rdtype, long points,int idx){
  int i,j;
  if(-KJ == dset->t){
    *(data + idx + i) = dset->j;
    idx = idx + 1;
  }
  else if(KJ == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kJ(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = longwrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
          *(data + idx + j) = kJ(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  // Write when the size of allocated buffer has been filled
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else if(strcmp(rdtype,"a")==0)
      H5Awrite(h5data, HDF5LONG, data);
    return 0;
  }
  return idx;
}

int floatwrite(hid_t h5data, K dset, double *data, char *rdtype, long points,int idx){
  int i,j;
  if(-KF == dset->t){
    *(data + idx + i) = dset->f;
    idx = idx + 1;
  }
  else if(KF == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kF(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = floatwrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
           *(data + idx + j) = kF(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  // Write when the size of allocated buffer has been filled
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else if(strcmp(rdtype,"a")==0)
      H5Awrite(h5data, HDF5FLOAT, data);
    return 0;
  }
  return idx;
}

int realwrite(hid_t h5data, K dset, float *data, char *rdtype, long points,int idx){
  int i,j;
  if(-KE == dset->t){
    *(data + idx + i) = dset->e;
    idx = idx + 1;
  }
  else if(KE == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kE(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = realwrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
           *(data + idx + j) = kE(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  // Write when the size of allocated buffer has been filled
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5REAL, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else if(strcmp(rdtype,"a")==0)
      H5Awrite(h5data, HDF5REAL, data);
    return 0;
  }
  return idx;
}

