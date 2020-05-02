#include <stdlib.h>
#include <string.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

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
    result = KF;
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
    default:
      return 0;
  }
}

// ktype (char) to ktypegroup (ktypegroup_t)
ktypegroup_t getKTypeGroup(char ktype){
  if(NULL != strchr("hijfebxpmdznuvt", ktype))
    return NUMERIC;
  if(NULL != strchr("csg", ktype))
    return STRING;
  return INVALID;
}
