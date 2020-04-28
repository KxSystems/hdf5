#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

#include "hdf5_write_utils.c"

// Write data to an attribute
EXP K hdf5writeAttrDataset(K fname, K dname, K aname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Cs][Ii]c", fname, dname, aname, kdims, ktype))
    return KNL;
  htri_t aexists;
  hid_t data, file,attr;
  char *filename = kdbGetString(fname);
  // Attempt to open file, error out if it doesn't exist
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return(krr((S)"file does not exist"));
  }
  char *dataname = kdbGetString(dname);
  // is dname a group/dataset, error out on issue
  data = H5Oopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return(krr((S)"dataset or group to write attribute to does not exist"));
  }
  char *attrname = kdbGetString(aname);
  // Check if type that is to be written to is numeric
  if(NUMERIC == getKType(ktype->g)){
    // Does the attribute already exist
    aexists = H5Aexists(file, attrname);
    if(aexists == 0)
      createNumericAttribute(data, attrname, kdims, ktype);
    // Could not evaluate if the attribute exists error out
    else if(aexists < 0){
      free(attrname);
      free(dataname);
      free(filename);
      H5Oclose(data);
      H5Fclose(file);
      return(krr((S)"check to ensure that attribute exists failed"));
    }
    attr = H5Aopen(data, attrname, H5P_DEFAULT);
    // write the kdb dset to the data file
    writenumeric(dset, attr, "a");
  }
  // Check if the type that is to be written to is character/symbol
  else if(STRING == getKType(ktype->g)){
    aexists = H5Aexists(file, attrname);
    if(aexists == 0)
      createStringAttribute(data, attrname, kdims);
    else if(aexists < 0){
      free(attrname);
      free(dataname);
      free(filename);
      H5Oclose(data);
      H5Fclose(file);
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
    H5Oclose(data);
    H5Fclose(file);
    return(krr((S)"Attempting to pass an unsupported datatype"));
  }
  // Clean up
  free(attrname);
  free(dataname);
  free(filename);
  H5Oclose(data);
  H5Aclose(attr);
  H5Fclose(file);
  return 0;
}

EXP K hdf5writeDataset(K fname, K dname, K dset, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Ii]c", fname, dname, kdims, ktype))
    return KNL;
  int isdset;
  hid_t data, file;
  char *filename = kdbGetString(fname);
  // Create a file is it does not exist
  htri_t filechk = H5Fis_hdf5(filename);
  if(filechk < 0)
    H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
  if(filechk == 0){
    free(filename);
    return krr((S)"This file already exists and is not a hdf5 file");
  }
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  char *dataname = kdbGetString(dname);
  // Create the appropriate dataset if it doesn't exist typed appropriately
  isdset = checkDataset(file, dataname);
  if(NUMERIC == getKType(ktype->g)){
    if(isdset == 0)
      createNumericDataset(file, dataname, kdims, ktype);
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    // write the kdb dset to the data file
    writenumeric(dset, data, "d");
  }
  else if(STRING == getKType(ktype->g)){
    if(isdset == 0)
      createStringDataset(file, dataname, kdims);
    data = H5Dopen(file, dataname, H5P_DEFAULT);
    writeChar(dset, data, "d");
  }
  else{
    free(dataname);
    free(filename);
    H5Fclose(file);
    return(krr((S)"Attempting to pass an unsupported datatype"));
  }
  free(dataname);
  free(filename);
  H5Dclose(data);
  H5Fclose(file);
  return 0;
}

