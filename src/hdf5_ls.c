#include <stdlib.h>
#include <stdio.h>
#include "hdf5_utils.h"

struct opdata {
  unsigned recurs;     // Recursion level (0=root)
  struct opdata *prev; // Pointer to previous opdata
  haddr_t addr;        // Group address
  K x;                 // type
  K y;                 // name
  K z;                 // values
};

int group_check(struct opdata *od, haddr_t target_addr);
herr_t opfunc(hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data);

ZK createDict(struct opdata* od){
  K keys = ktn(KS,3);
  kS(keys)[0]=ss("type");
  kS(keys)[1]=ss("name");
  kS(keys)[2]=ss("value");
  R xD(keys, knk(3,od->x,od->y,od->z));
}

EXP K hdf5ls(K fname){
  if(!kdbCheckType("C", fname))
    return KNL;
  hid_t file;
  H5O_info_t infobuf;
  struct opdata od;
  file = kdbH5Fopen(fname, H5F_ACC_RDONLY);
  if(file < 0)
    return krr((S)"file does not exist");
  H5Oget_info1(file,&infobuf);
  od.recurs = 0;
  od.prev = NULL;
  od.addr = infobuf.addr;
  od.x = ktn(KS,0);
  od.y = ktn(0,0);
  od.z = ktn(0,0);
  H5Literate(file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, opfunc, (void *)&od);
  H5Fclose(file);
  return createDict(&od);
}

// Utility functions

int group_check(struct opdata *od, haddr_t target_addr){
  if (od->addr == target_addr)
    return 1;
  else if (!od->recurs)
    return 0;
  else
    return group_check (od->prev, target_addr);
}

ZV addType(struct opdata* od,S type,S name,K dict){
  js(&od->x,type);
  jk(&od->y,ks(name));
  jk(&od->z,dict);
}

ZV addBasicType(struct opdata* od,S type,S name){
  addType(od,type,name,xD(ktn(KS,0),ktn(0,0)));
}

herr_t opfunc(hid_t loc_id, const char *name, const H5L_info_t * UNUSED(info), void *operator_data){
  herr_t return_val = 0;
  info=0;
  H5O_info_t infobuf;
  struct opdata *od = (struct opdata *) operator_data;
  H5Oget_info_by_name1(loc_id, name, &infobuf, H5P_DEFAULT);
  switch (infobuf.type) {
    case H5O_TYPE_GROUP:
      if( group_check(od, infobuf.addr) )
        printf ("Warning: Loop detected!\n");
      else {
        struct opdata nextod;
        nextod.recurs = od->recurs + 1;
        nextod.prev = od;
        nextod.addr = infobuf.addr;
        nextod.x = ktn(KS,0);
        nextod.y = ktn(0,0);
        nextod.z = ktn(0,0);
        return_val = H5Literate_by_name(loc_id, name, H5_INDEX_NAME,
                       H5_ITER_NATIVE, NULL, opfunc, (void *) &nextod,
                       H5P_DEFAULT);
        addType(od,(char*)"group",(char*)name,(!nextod.x->n)?r0(nextod.z),xD(nextod.x,nextod.y):createDict(&nextod));
      }
      break;
    case H5O_TYPE_DATASET:
      addBasicType(od,(char*)"dataset",(char*)name);
      break;
    case H5O_TYPE_NAMED_DATATYPE:
      addBasicType(od,(char*)"datatype",(char*)name);
      break;
    default:
      addBasicType(od,(char*)"unknown",(char*)name);
    }
  return return_val;
}
