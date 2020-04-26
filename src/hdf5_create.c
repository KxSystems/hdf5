/* --- Creation functionality ---
 * The functions contained within this script are used for creating
 * objects, links or h5 files using this interface.
*/

#include <stdlib.h>

#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5createAttr(K fname, K dname, K aname, K kdims, K ktype){
  // Disable the in built hdf5 errors
  disable_err();
  // Check types 
  if(!checkType("[Cs][Cs][Cs][Ii][Ccs]", fname, dname, aname, kdims, ktype))
    return KNL;
  hid_t file, data;
  htri_t aexists;
  // Determine the type which the attribute will have (symbol/char vs numeric)
  int  dtype = checkvalid(getkstring(ktype));
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
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
  aexists = isattr(data, attrname);
  if(aexists > 0){
    closeGroupData(file,dataname,data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"This attribute already exists for this dataset");
  }
  // create either string or numeric dataset depending on how data is being passed 
  if(dtype == 1)
    createsimpleattr(data, attrname, kdims, ktype);
  else if(dtype == 2)
    createstrattr(data, attrname, kdims);
  // Clean up
  closeGroupData(file,dataname,data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  return 0;
}


EXP K hdf5createDataset(K fname, K dname, K kdims, K ktype){
  // Disable errors from hdf5
  disable_err();
  if(!checkType("[Cs][Cs][Ii][Ccs]", fname, dname, kdims, ktype))
    return KNL;
  htri_t file_nm;
  hid_t file;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  int  dtype = checkvalid(getkstring(ktype));
  // Create a file is it does not exist
  file_nm = ish5(filename);
  if((file_nm == 0) || file_nm < 0)
    createfile(filename);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  // Create a numerical dataset with relevant dimensionality and type
  if(dtype == 1){
    // Check that the dataset can be created appropriately
    if(0==createsimpledataset(file, dataname, kdims, ktype)){
      // Clean up if dimensionality is not suited
      free(filename);
      free(dataname);
      H5Fclose(file);
      return krr((S)"Within the current numerical api datasets must have dimensionality < 3");
    }
  }
  // Create a string dataset with relevant dimensionality and type
  else if(dtype == 2){
    if(0==createstrdataset(file, dataname, kdims)){
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


EXP K hdf5createFile(K fname){
  // Disable errors from hdf5
  disable_err();
  if(!checkType("[Cs]",fname))
    return KNL;
  char *filename = getkstring(fname);
  createfile(filename);
  // Clean up
  free(filename);
  return 0;
}
