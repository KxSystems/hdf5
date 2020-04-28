#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5delAttr(K fname, K dname, K aname){
  if(!kdbCheckType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  hid_t file, data;
  char *filename, *dataname, *attrname;
  filename = kdbGetString(fname);
  if(H5Fis_hdf5(filename) <= 0){
    free(filename);
    return krr((S)"file does not exist or is not HDF5");
  }
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  dataname = kdbGetString(dname);
  data = H5Oopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Oclose(data);
    H5Fclose(file);
    return krr((S)"group/datset does not exist");
  }
  attrname = kdbGetString(aname);
  if(H5Adelete(data,attrname) < 0)
    krr((S)"attribute could not be deleted");
  free(filename);
  free(dataname);
  free(attrname);
  H5Oclose(data);
  H5Fclose(file);
  return KNL;
}
