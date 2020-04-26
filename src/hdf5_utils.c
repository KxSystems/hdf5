/* --- HDF5 utility functions --- */

#include "hdf5_utils.h"

// Retrieve hdf5 numeric types
hid_t hdf5typ_from_k(K ktype){
  hid_t val;
  char* kstring = getkstring(ktype);
  char typ = kstring[0];
  switch(typ){
    case 'i':
    case 'b':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      val = HDF5INT;
      break;
    case 'j':
    case 'p':
    case 'n':
      val = HDF5LONG;
      break;
    case 'f':
    case 'z':
      val = HDF5FLOAT;
      break;
    case 'e':
      val = HDF5REAL;
      break;
    case 'h':
      val = HDF5SHORT;
      break;
    case 'x':
      val = H5T_NATIVE_UCHAR;
      break;
    default:
      val = 0;
  }
  // Clean up
  free(kstring);
  return val;
}

// Disable errors from hdf5 side
void disable_err(void){H5Eset_auto1(NULL,NULL);}

// check if a file/attribute exists
htri_t ish5(char *filename){return H5Fis_hdf5(filename);}
htri_t isattr(hid_t data,char *attrname){return H5Aexists(data, attrname);}

// Create a file based on name
void createfile(char *filename){H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);}

// Create a string attribute
int createstrattr(hid_t data, char *attrname, K kdims){
  hid_t space, filetype;
  hsize_t dims[1];
  // Number of char arrays or symbols to within the string attribute
  int klen = kI(kdims)[0];
  dims[0] = klen;
  // Write data to FORTRAN type (this handles null termination)
  filetype = H5Tcopy(H5T_FORTRAN_S1);
  H5Tset_size(filetype, H5T_VARIABLE);
  space = H5Screate_simple(1, dims, NULL);
  H5Acreate2(data, attrname, filetype, space, H5P_DEFAULT,  H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(filetype);
  return 1;
}

// Used for the creation of simple attributes for types ijhef
int createsimpleattr(hid_t data, char *attrname, K kdims, K ktype){
  int i, rank;
  hid_t space;
  if(-KI==kdims->t)
    rank = 1;
  else
    rank = (I)kdims->n;
  // System limited to 3-D in current iteration
  if(rank>3)
    return 0;
  hsize_t dims[rank];
  for(i=0;i < rank;i++)
    dims[i] = kI(kdims)[i];
  // Define dimensionality of the space to be created
  space = H5Screate(H5S_SIMPLE);
  H5Sset_extent_simple(space, rank, dims, NULL);
  // Create the attribute dataset of appropriate type
  H5Acreate2(data, attrname, hdf5typ_from_k(ktype), space, H5P_DEFAULT, H5P_DEFAULT);
  // Clean up
  H5Sclose(space);
  return 1;
}

int createstrdataset(hid_t file, char *dataname, K kdims){
  hid_t space, filetype;
  hsize_t dims[1];
  int klen = kI(kdims)[0];
  dims[0] = klen;
  filetype = H5Tcopy(H5T_FORTRAN_S1);
  H5Tset_size(filetype, H5T_VARIABLE);
  space = H5Screate_simple(1, dims, NULL);
  H5Dcreate(file, dataname, filetype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(filetype);
  return 1;
}

// Used for the creation of simple datasets of type ijhef
int createsimpledataset(hid_t file, char *dataname, K kdims, K ktype){
  int i, rank;
  hid_t space, dtype;
  // Handle if someone passes an individual elements for dimensionality
  if(-KI==kdims->t)
    rank = 1;
  else
    rank = (I)kdims->n;
  if(rank>3)
    return 0;
  hsize_t dims[rank];
  for(i=0;i < rank;i++)
    dims[i] = kI(kdims)[i];
  space = H5Screate_simple(rank, dims, NULL);
  dtype = H5Tcopy(hdf5typ_from_k(ktype));
  H5Tset_order(dtype, H5T_ORDER_LE);
  H5Dcreate(file, dataname, dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  H5Sclose(space);
  H5Tclose(dtype);
  return 1;
}

// Check that the dataset exists
int checkdataset(hid_t file, char *dataname){
  // Create a buffer for datatype
  H5G_stat_t statbuf;
  // Commit the data information to the buffer
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  // Check the type of data is a dataset
  if(H5G_DATASET == statbuf.type)
    return 1;
  else
    return 0;
}

int checkgroup(hid_t file, char *groupname){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, groupname, 0, &statbuf);
  if(H5G_GROUP == statbuf.type)
    return 1;
  else
    return 0;
}

// Returns group/data object depending on format of the file denoted in dataname
hid_t isGroupData(hid_t file, char *dataname){
  hid_t data;
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  switch(statbuf.type){
  case H5G_GROUP:
    data = H5Gopen(file, dataname, H5P_DEFAULT);
    break;
  case H5G_DATASET:
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    break;
  default:
    data = 0;
    break;
  }
  return data;
}

// Close the group or datatype depending on object type
void closeGroupData(hid_t file, char *dataname,hid_t data){
  H5G_stat_t statbuf;
  H5Gget_objinfo(file, dataname, 0, &statbuf);
  switch(statbuf.type){
    case H5G_GROUP:
      H5Gclose(data);
      break;
    case H5G_DATASET:
      H5Dclose(data);
      break;
    default:
      break;
  }
}

// used to check what datatype is being passed in to make decisions on write path
int checkvalid(char *ktype){
  int flag=0;
  int i,j;
  char num[15] = "hijfebxpmdznuvt";
  char str[3] = "csg";
  for(i=0;i<15;i++){
    if(ktype[0] == num[i]){
      flag = 1;
      break;
    }
  }
  for(j=0;j<3;j++){
    if(ktype[0] == str[j]){
      flag = 2;
      break;
    }
  }
  return(flag);
}
