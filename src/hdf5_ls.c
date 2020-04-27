/* --- ls like function ---
 * Function to display the structure of a file 
*/

#include <stdlib.h>
#include "k.h"
#include "hdf5.h"
#include "kdb_utils.h"
#include "hdf5_utils.h"

struct opdata {
    unsigned recurs;     // Recursion level.  0=root
    struct opdata *prev; // Pointer to previous opdata
    haddr_t addr;        // Group address
};

int group_check(struct opdata *od, haddr_t target_addr);
herr_t opfunc(hid_t loc_id, const char *name, const H5L_info_t *info, void *operator_data);

EXP K hdf5ls(K fname){
  disable_err();
  if(!kdbCheckType("[Cs]",fname))
    return KNL;
  char *filename = kdbGetString(fname);
  hid_t file;
  H5O_info_t infobuf;
  struct opdata od;
  file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file<0){
    free(filename);
    H5Fclose(file);
    return krr((S)"file does not exist");
  }
  H5Oget_info1(file,&infobuf);
  od.recurs = 0;
  od.prev = NULL;
  od.addr = infobuf.addr;
  printf("/ {\n");
  H5Literate(file, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, opfunc, (void *)&od);
  printf("}\n");
  free(filename);
  H5Fclose(file);
  return 0;
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

herr_t opfunc(hid_t loc_id, const char *name, const H5L_info_t * UNUSED(info), void *operator_data){
  herr_t return_val = 0;
  info=0;
  H5O_info_t infobuf;
  struct opdata *od = (struct opdata *) operator_data;
  unsigned spaces = 2*(od->recurs+1);
  H5Oget_info_by_name1(loc_id, name, &infobuf, H5P_DEFAULT);
  printf ("%*s", spaces, "");
  switch (infobuf.type) {
    case H5O_TYPE_GROUP:
      printf ("Group: %s {\n", name);
      if( group_check(od, infobuf.addr) )
        printf ("%*s Warning: Loop detected!\n", spaces, "");
      else {
        struct opdata nextod;
        nextod.recurs = od->recurs + 1;
        nextod.prev = od;
        nextod.addr = infobuf.addr;
        return_val = H5Literate_by_name(loc_id, name, H5_INDEX_NAME,
                       H5_ITER_NATIVE, NULL, opfunc, (void *) &nextod,
                       H5P_DEFAULT);
      }
      printf ("%*s}\n", spaces, "");
      break;
    case H5O_TYPE_DATASET:
      printf ("Dataset: %s\n", name);
      break;
    case H5O_TYPE_NAMED_DATATYPE:
      printf ("Datatype: %s\n", name);
      break;
    default:
      printf ( "Unknown: %s\n", name);
    }
  return return_val;
}
