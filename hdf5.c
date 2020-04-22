#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#define EXP __declspec(dllexport)
#else
#define EXP
#endif

#include <hdf5.h>
#include "headers/k.h"
#include "headers/q_hdf5.h"

#include "src/utils/utils.c"
#include "src/utils/hdf5_type_utils.c"
#include "src/utils/hdf5_utils.c"
#include "src/hdf5_create.c"
#include "src/hdf5_general.c"
#include "src/utils/hdf5_read_utils.c"
#include "src/hdf5_read.c"
#include "src/utils/hdf5_write_utils.c"
#include "src/hdf5_write.c"
#include "src/hdf5_ls.c"
#include "src/hdf5_groups.c"
#include "src/hdf5_links.c"
#include "src/hdf5_del.c"


/* --- Naming Convention ---
 * The following naming convention is used throughout the source code
 * these are the elements that repeat
 * fname = file name
 * dname = data name
 * dset  = q dataset
 * gname = group name
 * aname = attribute name
 * kdims = K dimensionality as a list of ints
 * ktype = K type as a single char
 * lname = link name
*/

