/* --- Retrieve hdf5 numeric types --- */
hid_t hdf5typ_from_k(K ktype){
  hid_t val;
  char* kstring = getkstring(ktype);
  if(strcmp(kstring,"i")==0)
    val = HDF5INT;
  else if(strcmp(kstring,"b")==0)
    val = HDF5INT;
  else if(strcmp(kstring,"j")==0)
    val = HDF5LONG;
  else if(strcmp(kstring,"f")==0)
    val = HDF5FLOAT;
  else if(strcmp(kstring,"e")==0)
    val = HDF5REAL;
  else if(strcmp(kstring,"h")==0)
    val = HDF5SHORT;
  else if(strcmp(kstring,"x")==0)
    val = H5T_NATIVE_UCHAR;
  else
    val = 0;
  // Clean up
  free(kstring);
  return val;
  }
