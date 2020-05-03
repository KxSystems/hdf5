#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// declare write utils
K writeNumeric(K dset, hid_t loc, hid_t space, hid_t ntype);
K writeString(K dset, hid_t loc, hid_t space, hid_t ntype);

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Ii]c", fname, dname, kdims, ktype))
    return KNL;
  hid_t file, data, space;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname;
  hssize_t rank, i;
  hsize_t *dims;

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
  // check datatypes
  htype = k2hType(ktype->g);
  dtype = H5Dget_type(data);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  if(htype != ntype)
    return krr((S)"type does not match");
  space = H5Dget_space(data);
  if(space < 0)
    return krr((S)"error opening dataspace");
  // check rank
  rank = kdims->n;
  if(rank != H5Sget_simple_extent_npoints(space))
    return krr((S)"rank does no match");
  // check dims
  dims = calloc(rank, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < rank; i++)
    if(kJ(kdims)[i] != (J)dims[i])
      return krr((S)"dimensions do not match");
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    writeNumeric(dset, data, space, ntype);
  else if(gtype == STRING)
    writeString(dset, data, space, ntype);
  else{
    krr((S)"unsupported datatype");
  }
  H5Dclose(data);
  H5Sclose(space);
  return KNL;
}

EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Cs][Ii]c", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data, attr, space;
  hid_t htype, dtype, ntype;
  ktypegroup_t gtype;
  char *filename, *dataname, *attrname;
  hssize_t rank, i;
  hsize_t *dims;

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
  attr = H5Aopen(data, attrname, H5P_DEFAULT);
  free(attrname);
  H5Oclose(data);
  if(attr < 0)
    return krr((S)"error opening attribute");
  // check datatypes
  htype = k2hType(ktype->g);
  dtype = H5Dget_type(attr);
  ntype = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  if(htype != ntype)
    return krr((S)"type does not match");
  space = H5Aget_space(attr);
  if(space < 0)
    return krr((S)"error opening dataspace");
  // check rank
  rank = kdims->n;
  if(rank != H5Sget_simple_extent_npoints(space))
    return krr((S)"rank does no match");
  // check dims
  dims = calloc(rank, sizeof(hssize_t));
  H5Sget_simple_extent_dims(space, dims, NULL);
  for(i = 0; i < rank; i++)
    if(kJ(kdims)[i] != (J)dims[i])
      return krr((S)"dimensions do not match");
  gtype = getKTypeGroup(ktype->g);
  if(gtype == NUMERIC)
    writeNumeric(dset, attr, space, ntype);
  else if(gtype == STRING)
    writeString(dset, attr, space, ntype);
  else{
    krr((S)"unsupported datatype");
  }
  H5Aclose(attr);
  H5Sclose(space);
  return KNL;
}

// declare write utils
K writeNumeric(K dset, hid_t loc, hid_t space, hid_t ntype);
K writeString(K dset, hid_t loc, hid_t space, hid_t ntype);

// define write utils

long copyNumeric(hid_t UNUSED(loc), K UNUSED(dset), void *UNUSED(dset_data), hid_t UNUSED(ntype), long UNUSED(idx)){
  return 0;
}

K writeNumeric(K dset, hid_t loc, hid_t space, hid_t ntype){
  hid_t status;
  long npoints, idx;
  void *dset_data;
  npoints = H5Sget_simple_extent_npoints(space);
  dset_data = calloc(npoints, sizeof(double)); // need size of ntype
  idx = copyNumeric(loc, dset, dset_data, ntype, 0);
  if(idx != npoints)
    return krr("invalid data");
  status = H5Dwrite(loc, ntype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data); // need generic write func
  if(status < 0)
    return krr("error writing to dataset");
  free(dset_data);
  return KNL;
}

K writeString(K UNUSED(dset), hid_t UNUSED(loc), hid_t UNUSED(space), hid_t UNUSED(ntype)){
  return KNL;
}





/*

  int i, j;
  if(idx < 0)
    return idx;
  if(dset->t == KG){
    for(i = 0; i < dset->n; ++i)
      dset_data[idx + i] = kG(dset)[i];
    idx += dset->n;
  }
  else if(dset->t == 0){
    for(i = 0; i < dset->n; ++i)
  }
  else
    return -1;
  return idx;
}






















// utils

int longwrite (hid_t h5data, K dset, long   *data, char *rdtRype, long points, int idx);
int intwrite  (hid_t h5data, K dset, int    *data, char *rdtype, long points, int idx);
int shortwrite(hid_t h5data, K dset, short  *data, char *rdtype, long points, int idx);
int realwrite (hid_t h5data, K dset, float  *data, char *rdtype, long points, int idx);
int floatwrite(hid_t h5data, K dset, double *data, char *rdtype, long points, int idx);
int bytewrite (hid_t h5data, K dset, unsigned char *data, char *rdtype, long points,int idx);


static void writeFloat(K dset, hid_t data, char *rdtype){
  hid_t space;
  // Get the dataspace identifier based on if attr/dset
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  // Allocate the memory needed for writing to hdf5
}

static void writeByte(K dset, hid_t data, char *rdtype){
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  unsigned char *dset_data = (unsigned char *)malloc(points *sizeof(unsigned char));
  bytewrite(data, dset, dset_data, rdtype, points, 0);
  free(dset_data);
  H5Sclose(space);
}

static void writeInt(K dset, hid_t data, char *rdtype){
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  int *dset_data = (int *)malloc(points *sizeof(int));
  intwrite(data, dset, dset_data, rdtype, points, 0);
  free(dset_data);
  H5Sclose(space);
}

static void writeLong(K dset, hid_t data, char *rdtype){
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  long *dset_data = (long *)malloc(points *sizeof(long));
  longwrite(data, dset, dset_data, rdtype, points, 0);
  free(dset_data);
  H5Sclose(space);
}

static void writeReal(K dset, hid_t data, char *rdtype){
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  long points = H5Sget_simple_extent_npoints(space);
  float *dset_data = (float *)malloc(points *sizeof(float));
  realwrite(data, dset, dset_data, rdtype, points,0);
  free(dset_data);
  H5Sclose(space);
}

static void writeShort(K dset, hid_t data, char *rdtype){
  hid_t space;
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  long  points = H5Sget_simple_extent_npoints(space);
  short *dset_data = (short *)malloc(points *sizeof(short));
  shortwrite(data, dset, dset_data, rdtype, points, 0);
  H5Sclose(space);
  free(dset_data);
}

static void writeNumeric(K dset, hid_t data, char *data_attr){
  hid_t dtype,ntype;
  // Retrieve an identifier to the dataset type
  if(strcmp("d",data_attr)==0)
    dtype = H5Dget_type(data);
  else
    dtype = H5Aget_type(data);
  // Retrieve the underlying C representation of the type
  ntype = H5Tget_native_type(dtype,H5T_DIR_ASCEND);
  if(H5Tequal(ntype, HDF5INT))
    writeInt(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5LONG))
    writeLong(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5BYTE))
    writeByte(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5FLOAT))
    writeFloat(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5REAL))
    writeReal(dset, data, data_attr);
  else if(H5Tequal(ntype, HDF5SHORT))
    writeShort(dset, data, data_attr);
  H5Tclose(dtype);
  H5Tclose(ntype);
}

static void writeString(K dset, hid_t data, char *data_attr){
  long i,arr;
  // Create a variable length memory space to store the K chars
  hid_t memtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(memtype, H5T_VARIABLE);
  arr = dset->n;
  char **wdata = malloc(sizeof(char*) * arr);
  for(i=0;i<arr;i++)
    wdata[i]= kdbGetString(kK(dset)[i]);
  if(strcmp(data_attr,"d")==0)
    H5Dwrite(data, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
  else
    H5Awrite(data, memtype, wdata);
  free(wdata);
  H5Tclose(memtype);
}

int intwrite(hid_t h5data, K dset, int *data, char *rdtype, long points,int idx){
  int i=0, j;
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
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, HDF5INT, data);
    return 0;
  }
  return idx;
}

int shortwrite(hid_t h5data, K dset, short *data, char *rdtype, long points,int idx){
  int i=0, j;
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
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5SHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, HDF5SHORT, data);
    return 0;
  }
  return idx;
}

int longwrite(hid_t h5data, K dset, long *data, char *rdtype, long points,int idx){
  int i=0, j;
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
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5LONG, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, HDF5LONG, data);
    return 0;
  }
  return idx;
}

int floatwrite(hid_t h5data, K dset, double *data, char *rdtype, long points,int idx){
  int i=0, j;
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
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, HDF5FLOAT, data);
    return 0;
  }
  return idx;
}

int realwrite(hid_t h5data, K dset, float *data, char *rdtype, long points,int idx){
  int i=0, j;
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
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, HDF5REAL, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, HDF5REAL, data);
    return 0;
  }
  return idx;
}

int bytewrite(hid_t h5data, K dset, unsigned char *data, char *rdtype, long points,int idx){
  int i=0, j;
  if(-KG == dset->t){
    *(data + idx + i) = dset->g;
    idx = idx + 1;
  }
  else if(KG == dset->t){
    for(i = 0; i < dset -> n; i++)
      *(data + idx + i) = kG(dset)[i];
    idx = idx + dset -> n;
  }
  else{
    for(i=0;i<dset->n;i++){
      K element = kK(dset)[i];
      if(0==element->t)
        idx = bytewrite(h5data,element,data,rdtype,points,idx);
      else{
        for(j=0;j<element->n;j++){
          *(data + idx + j) = kG(element)[j];
        }
      idx = idx+element->n;
      }
    }
  }
  if(((I)points)==idx){
    if(strcmp(rdtype,"d")==0)
      H5Dwrite(h5data, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,data);
    else
      H5Awrite(h5data, H5T_NATIVE_UCHAR, data);
    return 0;
  }
  return idx;
}

*/
