#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5delAttr(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data;
  char *filename = kdbGetString(fname);
  char *dataname = kdbGetString(dname);
  char *attrname = kdbGetString(aname);
  if(H5Fis_hdf5(filename) <= 0){
    free(filename);
    return krr((S)"file does not exist or is not HDF5");
  }
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  data = openGroupData(file, dataname);
  if(data < 0){
    closeGroupData(file, dataname, data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"group/datset does not exist");
  }
  if(H5Adelete(data,attrname) < 0)
    krr((S)"attribute could not be deleted");
  closeGroupData(file, dataname, data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  return KNL;
}
