#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

static void writenumeric(K dset, hid_t data, char *data_attr);
static void writeChar(K dset, hid_t data, char *data_attr);

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Ii]c", fname, dname, kdims, ktype))
    return KNL;
  hid_t data, file;
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
  if     (NUMERIC == getKTypeGroup(ktype->g))
    writenumeric(dset, data, "d");
  else if(STRING == getKTypeGroup(ktype->g))
    writeChar(dset, data, "d");
  else{
    return krr((S)"unsupported datatype");
    H5Dclose(data);
  }
  H5Dclose(data);
  return KNL;
}

EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Cs][Ii]c", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t data, file,attr;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file does not exist");
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
  if(NUMERIC == getKTypeGroup(ktype->g))
    writenumeric(dset, attr, "a");
  else if(STRING == getKTypeGroup(ktype->g))
    writeChar(dset, attr, "a");
  else{
    return krr((S)"unsupported datatype");
    H5Aclose(attr);
  }
  H5Aclose(attr);
  return KNL;
}

// utils

/* Function prototypes for writes */
int longwrite (hid_t h5data, K dset, long   *data, char *rdtype, long points, int idx);
int intwrite  (hid_t h5data, K dset, int    *data, char *rdtype, long points, int idx);
int shortwrite(hid_t h5data, K dset, short  *data, char *rdtype, long points, int idx);
int realwrite (hid_t h5data, K dset, float  *data, char *rdtype, long points, int idx);
int floatwrite(hid_t h5data, K dset, double *data, char *rdtype, long points, int idx);
int bytewrite (hid_t h5data, K dset, unsigned char *data, char *rdtype, long points,int idx);

/* Writing to numeric datatypes */

static void writeFloat(K dset, hid_t data, char *rdtype){
  hid_t space;
  // Get the dataspace identifier based on if attr/dset
  if(strcmp(rdtype,"d")==0)
    space = H5Dget_space(data);
  else
    space = H5Aget_space(data);
  // Allocate the memory needed for writing to hdf5
  long points = H5Sget_simple_extent_npoints(space);
  double *dset_data = (double *)malloc(points *sizeof(double));
  floatwrite(data, dset, dset_data, rdtype, points, 0);
  // Clean up
  free(dset_data);
  H5Sclose(space);
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

static void writenumeric(K dset, hid_t data, char *data_attr){
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

/* Write character data to HDF5 */
static void writeChar(K dset, hid_t data, char *data_attr){
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

/* Functionality for writing iteratively writing to numeric types*/

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
