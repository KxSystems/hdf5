#define KNL (K) 0
#define HDF5FLOAT H5T_NATIVE_DOUBLE 
#define HDF5INT   H5T_NATIVE_INT
#define HDF5LONG  H5T_NATIVE_LONG
#define HDF5REAL  H5T_NATIVE_FLOAT
#define HDF5SHORT H5T_NATIVE_SHORT
#define HDF5CHAR  H5T_C_S1
#ifdef __GNUC__
#  define UNUSED(x) x __attribute__((__unused__))
#else
#  define UNUSED(x) x
#endif
