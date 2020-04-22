/* --- Retrieve hdf5 numeric types --- */
hid_t hdf5typ_from_k(K ktype){
  hid_t val;
  char* kstring = getkstring(ktype);  
  char typ = kstring[0];
  switch(typ){
    case 'i':
    case 'b':
    case 'd':
    case 'u':
    case 'v':
    case 't':
      val = HDF5INT;
      break;
    case 'j':
    case 'p':
    case 'n':
      val = HDF5LONG;
      break;
    case 'f':
    case 'z':
      val = HDF5FLOAT;
      break;
    case 'e':
      val = HDF5REAL;
      break;
    case 'h':
      val = HDF5SHORT;
      break;
    case 'x':
      val = H5T_NATIVE_UCHAR;
      break;
    default:
      val = 0;
  }
  // Clean up
  free(kstring);
  return val;
}
