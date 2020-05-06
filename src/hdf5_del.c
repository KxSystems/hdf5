#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

EXP K hdf5delAttr(K fname, K dname, K aname){
  if(!kdbCheckType("CCC", fname, dname, aname))
    return KNL;
  hid_t file, data, status;
  char *attrname;
  file = kdbH5Fopen(fname, H5F_ACC_RDWR);
  if(file < 0)
    return krr((S)"error opening file");
  data = kdbH5Oopen(file, dname);
  H5Fclose(file);
  if(data < 0)
    return krr((S)"error opening dataset/group");
  attrname = kdbGetString(aname);
  status = H5Adelete(data, attrname);
  free(attrname);
  H5Oclose(data);
  if(status < 0)
    return krr((S)"error deleting attribute");
  return KNL;
}
