/* --- Deletion functionality --- */

#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5delAttr(K fname, K dname, K aname){
  disable_err();
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K res;
  hid_t file, data;
  htri_t file_nm;
  herr_t adel;
  char *filename = getkstring(fname);
  file_nm = ish5(filename);
  if((file_nm == 0) || file_nm < 0){
    free(filename);
    return krr((S)"File does not exist or is not HDF5");
  }
  // Open appropriate HDF5 file
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  // Is the object that you are deleting from an attribute or dataset
  data = isGroupData(file,dataname);
  if(data < 0){
    closeGroupData(file,dataname,data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"The group/object does not exist");
  }
  adel = H5Adelete(data,attrname);
  // Indicate if deletion has been
  if(adel >= 0)
    res = kp("Successfully deleted attribute");
  else
    res = krr((S)"Attribute could not be deleted");
  // Clean up
  closeGroupData(file, dataname, data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  return res;
}
