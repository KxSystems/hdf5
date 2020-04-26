/* --- Creation functionality ---
 * The functions contained within this script are used for creating
 * objects, links or h5 files using this interface.
*/

#include "k.h"
#include "util.h"

EXP K hdf5createAttr(K fname, K dname, K aname, K kdims, K ktype);

EXP K hdf5createDataset(K fname, K dname, K kdims, K ktype);

EXP K hdf5createFile(K fname);
