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
  K res;
  hid_t file;
  char *filename = kdbGetString(fname);
  file = H5Fopen(filename,  H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *linkname = kdbGetString(lname);
  char *targetnm = kdbGetString(tname);
  char *tobjname = kdbGetString(tobj);
  // Create link to external location
  if(H5Lcreate_external(targetnm, tobjname, file, linkname, H5P_DEFAULT, H5P_DEFAULT)<0)
    res = krr((S)"Creation of external link unsuccessful");
  else
    res = 0;
  // Clean up
  free(filename);
  free(linkname);
  free(targetnm);
  free(tobjname);
  H5Fclose(file);
  return res;
}

// Create a hard link from a defined location within a file (this object must exist)
// and another location
/* oname = name of the object being linked to */
/* lname = linking path name */
EXP K hdf5createHard(K fname, K oname, K lname){
  if(!kdbCheckType("CCC", fname, oname, lname))
    return KNL;
  K res;
  hid_t file;
  char *filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *objname  = kdbGetString(oname);
  char *linkname = kdbGetString(lname);
  // Create hard link between two physical locations within a file
  if(H5Lcreate_hard(file, objname, H5L_SAME_LOC, linkname, H5P_DEFAULT, H5P_DEFAULT)<0)
    res = krr((S)"Creation of hard link unsuccessful");
  else
    res = 0;
  free(filename);
  free(objname);
  free(linkname);
  H5Fclose(file);
  return res;
}

// Create a soft link between a two locations
/* tpath = target path (may be dangling) */
/* lpath = soft link path */
EXP K hdf5createSoft(K fname,K tpath,K lname){
  if(!kdbCheckType("CCC", fname, tpath, lname))
    return KNL;
  K res;
  hid_t file;
  char *filename = kdbGetString(fname);
  char *tarname  = kdbGetString(tpath);
  char *linkname = kdbGetString(lname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  // Create a soft/symbolic link between two locations within a file
  if(H5Lcreate_soft(tarname, file, linkname, H5P_DEFAULT, H5P_DEFAULT)<0)
    res = krr((S)"Creation of soft link unsuccessful");
  else
    res = 0;
  free(filename);
  free(tarname);
  free(linkname);
  H5Fclose(file);
  return res;
}

// Delete a hard/soft or external link
EXP K hdf5delLink(K fname, K lname){
  if(!kdbCheckType("CC", fname, lname))
    return KNL;
  K res;
  hid_t file;
  char *filename = kdbGetString(fname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  char *linkname = kdbGetString(lname);
  if(H5Ldelete(file, linkname, H5P_DEFAULT)<0)
    res = krr((S)"could not delete specified link");
  else
    res = kp("successfully deleted the link");
  free(filename);
  free(linkname);
  H5Fclose(file);
  return res;
}
