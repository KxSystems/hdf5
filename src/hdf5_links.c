#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// create soft link to an external file
EXP K hdf5createExternal(K linkfile, K linkpath, K targetfile, K targetpath){
  if(!kdbCheckType("CCCC", linkfile, linkpath, targetfile, targetpath))
    return KNL;
  hid_t file, status;
  char *linkfilename, *linkpathname, *targetfilename, *targetpathname;
  linkfilename = kdbGetString(linkfile);
  file = H5Fopen(linkfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(linkfilename);
  if(file < 0)
    return krr((S)"error opening file");
  linkpathname = kdbGetString(linkpath);
  targetfilename = kdbGetString(targetfile);
  targetpathname = kdbGetString(targetpath);
  status = H5Lcreate_external(targetfilename, targetpathname, file, linkfilename, H5P_DEFAULT, H5P_DEFAULT);
  free(linkpathname);
  free(targetfilename);
  free(targetpathname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating external link");
  return KNL;
}

// create hard link within file (target must exist)
EXP K hdf5createHard(K linkfile, K linkpath, K targetpath){
  if(!kdbCheckType("CCC", linkfile, linkpath, targetpath))
    return KNL;
  hid_t file, status;
  char *linkfilename, *linkpathname, *targetpathname;
  linkfilename = kdbGetString(linkfile);
  file = H5Fopen(linkfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(linkfilename);
  if(file < 0)
    return krr((S)"error opening file");
  linkpathname = kdbGetString(linkpath);
  targetpathname = kdbGetString(targetpath);
  status = H5Lcreate_hard(file, targetpathname, H5L_SAME_LOC, linkpathname, H5P_DEFAULT, H5P_DEFAULT);
  free(linkpathname);
  free(targetpathname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating hard link");
  return KNL;
}

// create soft link within file (target may be dangling)
EXP K hdf5createSoft(K linkfile, K linkpath, K targetpath){
  if(!kdbCheckType("CCC", linkfile, linkpath, targetpath))
    return KNL;
  hid_t file, status;
  char *linkfilename, *linkpathname, *targetpathname;
  linkfilename = kdbGetString(linkfile);
  file = H5Fopen(linkfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  if(file < 0)
    return krr((S)"error opening file");
  linkpathname = kdbGetString(linkpath);
  targetpathname = kdbGetString(targetpath);
  status = H5Lcreate_soft(targetpathname, file, linkpathname, H5P_DEFAULT, H5P_DEFAULT);
  free(linkpathname);
  free(targetpathname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error creating soft link");
  return KNL;
}

// delete link
EXP K hdf5delLink(K linkfile, K linkpath){
  if(!kdbCheckType("CC", linkfile, linkpath))
    return KNL;
  hid_t file, status;
  char *linkfilename, *linkpathname;
  linkfilename = kdbGetString(linkfile);
  file = H5Fopen(linkfilename, H5F_ACC_RDWR, H5P_DEFAULT);
  free(linkfilename);
  if(file < 0)
    return krr((S)"error opening file");
  linkpathname = kdbGetString(linkpath);
  status = H5Ldelete(file, linkpathname, H5P_DEFAULT);
  free(linkpathname);
  H5Fclose(file);
  if(status < 0)
    krr((S)"error deleting link");
  return KNL;
}
