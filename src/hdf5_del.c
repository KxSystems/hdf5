/* --- Deletion functionality --- */

#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5delAttr(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K res;
  hid_t file, data;
  herr_t adel;
  char *filename = kdbGetString(fname);
  if(H5Fis_hdf5(filename) <= 0){
    free(filename);
    return krr((S)"File does not exist or is not HDF5");
  }
  // Open appropriate HDF5 file
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  char *dataname = kdbGetString(dname);
  char *attrname = kdbGetString(aname);
  // Is the object that you are deleting from an attribute or dataset
  data = openGroupData(file, dataname);
  if(data < 0){
    closeGroupData(file, dataname, data);
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
