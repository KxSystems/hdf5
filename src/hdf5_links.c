#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// Create a symbolic/soft link between a paths within a local hdf5 file
// and an external link in another hdf5 file.
/* tname = target file path */
/* tobj = object path in target file */
EXP K hdf5createExternal(K fname, K lname, K tname, K tobj){
  if(!kdbCheckType("CCCC", fname, lname, tname, tobj))
    return KNL;
  hid_t file, status;
  char *filename, *linkname, *targetnm, *tobjname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename,  H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  linkname = kdbGetString(lname);
  targetnm = kdbGetString(tname);
  tobjname = kdbGetString(tobj);
  status = H5Lcreate_external(targetnm, tobjname, file, linkname, H5P_DEFAULT, H5P_DEFAULT);
  free(linkname);
  free(targetnm);
  free(tobjname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating external link");
  return KNL;
}

// Create a hard link from a defined location within a file (this object must exist)
// and another location
/* oname = name of the object being linked to */
/* lname = linking path name */
EXP K hdf5createHard(K fname, K oname, K lname){
  if(!kdbCheckType("CCC", fname, oname, lname))
    return KNL;
  hid_t file, status;
  char *filename, *objname, *linkname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  objname  = kdbGetString(oname);
  linkname = kdbGetString(lname);
  status = H5Lcreate_hard(file, objname, H5L_SAME_LOC, linkname, H5P_DEFAULT, H5P_DEFAULT);
  free(objname);
  free(linkname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating hard link");
  return KNL;
}

// Create a soft link between a two locations
/* tpath = target path (may be dangling) */
/* lpath = soft link path */
EXP K hdf5createSoft(K fname, K tpath, K lname){
  if(!kdbCheckType("CCC", fname, tpath, lname))
    return KNL;
  hid_t file, status;
  char *filename, *tarname, *linkname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0)
    return krr((S)"error opening file");
  tarname  = kdbGetString(tpath);
  linkname = kdbGetString(lname);
  status = H5Lcreate_soft(tarname, file, linkname, H5P_DEFAULT, H5P_DEFAULT);
  free(tarname);
  free(linkname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating soft link");
  return KNL;
}

// delete link
EXP K hdf5delLink(K fname, K lname){
  if(!kdbCheckType("CC", fname, lname))
    return KNL;
  hid_t file, status;
  char *filename, *linkname;
  filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(filename);
  if(file < 0)
    return krr((S)"error opening file");
  linkname = kdbGetString(lname);
  status = H5Ldelete(file, linkname, H5P_DEFAULT);
  free(linkname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error deleting link");
  return KNL;
}
