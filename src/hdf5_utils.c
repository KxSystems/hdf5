#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

// string types
hid_t varstringtype;
void initvarstringtype(){
  varstringtype = H5Tcopy(H5T_C_S1);
  H5Tset_size(varstringtype, H5T_VARIABLE);
}

// error handler info
herr_t (*err_func)(void*);
void *err_data;
void initerror(){
  H5Eget_auto1(&err_func, &err_data);
}
void errorOn(){
  H5Eset_auto1(err_func, err_data);
}
void errorOff(){
  H5Eset_auto1(NULL, NULL);
}

// manipulate attribute functions to have the same signature as equivalent dataset functions
hid_t kdbH5Acreate(hid_t attr, const char *name, hid_t type, hid_t space, hid_t UNUSED(lcpl), hid_t cpl, hid_t apl){
  return H5Acreate(attr, name, type, space, cpl, apl);
}
herr_t kdbH5Aread(hid_t attr, hid_t memtype, hid_t UNUSED(mspace), hid_t UNUSED(fspace), hid_t UNUSED(pl), void *buf){
  return H5Aread(attr, memtype, buf);
}
herr_t kdbH5Awrite(hid_t attr, hid_t memtype, hid_t UNUSED(mspace), hid_t UNUSED(fspace), hid_t UNUSED(pl), const void *buf){
  return H5Awrite(attr, memtype, buf);
}

// open functions
hid_t kdbH5Fopen(K name, unsigned flags){
  hid_t res;
  char *namestr;
  namestr = kdbGetString(name);
  res = H5Fopen(namestr, flags, H5P_DEFAULT);
  free(namestr);
  return res;
}
hid_t kdbH5Dopen(hid_t loc, K name){
  hid_t res;
  char *namestr;
  namestr = kdbGetString(name);
  res = H5Dopen(loc, namestr, H5P_DEFAULT);
  free(namestr);
  return res;
}
hid_t kdbH5Aopen(hid_t loc, K name){
  hid_t res;
  char *namestr;
  namestr = kdbGetString(name);
  res = H5Aopen(loc, namestr, H5P_DEFAULT);
  free(namestr);
  return res;
}
hid_t kdbH5Oopen(hid_t loc, K name){
  hid_t res;
  char *namestr;
  namestr = kdbGetString(name);
  res = H5Oopen(loc, namestr, H5P_DEFAULT);
  free(namestr);
  return res;
}


// htype (hid_t) to ktype (H)
H h2kType(hid_t htype){
  hid_t ntype;
  H result;
  ntype = H5Tget_native_type(htype, H5T_DIR_ASCEND);
       if(H5Tequal(ntype, H5T_NATIVE_CHAR))
    result = KC;
  else if(H5Tequal(ntype, H5T_NATIVE_SHORT))
    result = KH;
  else if(H5Tequal(ntype, H5T_NATIVE_INT))
    result = KI;
  else if(H5Tequal(ntype, H5T_NATIVE_LONG))
    result = (sizeof(long) == 16) ? KI : KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_LLONG))
    result = KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_UCHAR))
    result = KG;
  else if(H5Tequal(ntype, H5T_NATIVE_USHORT))
    result = KH;
  else if(H5Tequal(ntype, H5T_NATIVE_UINT))
    result = KI;
  else if(H5Tequal(ntype, H5T_NATIVE_ULONG))
    result = (sizeof(long) == 16) ? KI : KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_ULLONG))
    result = KJ;
  else if(H5Tequal(ntype, H5T_NATIVE_FLOAT))
    result = KE;
  else if(H5Tequal(ntype, H5T_NATIVE_DOUBLE))
    result = KF;
  else if(H5Tequal(ntype, H5T_NATIVE_B8))
    result = KG;
  else if(H5Tequal(ntype, H5T_NATIVE_B16))
    result = KH;
  else if(H5Tequal(ntype, H5T_NATIVE_B32))
    result = KI;
  else if(H5Tequal(ntype, H5T_NATIVE_B64))
    result = KJ;
   else
     result = 0;
  H5Tclose(ntype);
  return result;
}

// ktype (char) to htype (hid_t)
hid_t k2hType(char ktype){
  switch(ktype){
    case 'c':
      return H5T_NATIVE_CHAR;
    case 'b':
    case 'x':
      return H5T_NATIVE_UCHAR;
    case 'h':
      return H5T_NATIVE_SHORT;
    case 'i':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      return H5T_NATIVE_INT;
    case 'j':
    case 'p':
    case 'n':
      return H5T_NATIVE_LLONG;
    case 'e':
      return H5T_NATIVE_FLOAT;
    case 'f':
    case 'z':
      return H5T_NATIVE_DOUBLE;
    case 's':
    case 'C':
      return varstringtype;
    default:
      return 0;
  }
}

// ktype (char) to ktypegroup (ktypegroup_t)
ktypegroup_t getKTypeGroup(char ktype){
  if(NULL != strchr("chijfebxpmdznuvt", ktype))
    return NUMERIC;
  if(NULL != strchr("Csg", ktype))
    return STRING;
  return INVALID;
}
