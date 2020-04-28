/* --- Creation functionality ---
 * The functions contained within this script are used for creating
 * objects, links or h5 files using this interface.
*/

#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5createFile(K fname){
  if(!kdbCheckType("[Cs]", fname))
    return KNL;
  char *filename = kdbGetString(fname);
  H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
  free(filename);
  return 0;
}

EXP K hdf5createDataset(K fname, K dname, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs]Ic", fname, dname, kdims, ktype))
    return KNL;
  hid_t file;
  kdata_t dtype;
  char *filename = kdbGetString(fname);
  char *dataname = kdbGetString(dname);
  if(H5Fis_hdf5(filename) <= 0) // create file if it does not exist
    H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  dtype = checkvalid(ktype->g);
  if(dtype == NUMERIC){ // create numerical dataset
    if(0==createNumericDataset(file, dataname, kdims, ktype)){
      // Clean up
      free(filename);
      free(dataname);
      H5Fclose(file);
      return krr((S)"Within the current numerical api datasets must have dimensionality < 3");
    }
  }
  else if(dtype == STRING){ // create string dataset
    if(0==createStringDataset(file, dataname, kdims)){
      // Clean up
      free(filename);
      free(dataname);
      H5Fclose(file);
      return krr((S)"Within the current api string datasets must have dimensionality < 2");
    }
  }
  // Clean up
  free(filename);
  free(dataname);
  H5Fclose(file);
  return 0;
}

EXP K hdf5createAttr(K fname, K dname, K aname, K kdims, K ktype){
  if(!kdbCheckType("[Cs][Cs][Cs][Ii]c", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data;
  kdata_t  dtype;
  char *filename = kdbGetString(fname);
  char *dataname = kdbGetString(dname);
  char *attrname = kdbGetString(aname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  // Error if the file doesn't exist attributes only added to existing groups/dsets
  if(file < 0){
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"file does not exist");
  }
  // Retrieve the group/data to which the attr is to be written (error if it doesn't exist)
  data = isGroupData(file,dataname);
  if(data < 0){
    free(filename);
    free(dataname);
    free(attrname);
    H5Fclose(file);
    return krr((S)"dataset/group which attribute is to be written to does not exist");
  }
  // Check if attribute already exists, error if it does
  if(H5Aexists(data, attrname) > 0){
    closeGroupData(file,dataname,data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"This attribute already exists for this dataset");
  }
  dtype = checkvalid(ktype->g);
  if(dtype == NUMERIC)
    createNumericAttr(data, attrname, kdims, ktype);
  else if(dtype == STRING)
    createStringAttr(data, attrname, kdims);
  // Clean up
  closeGroupData(file,dataname,data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  return 0;
}
