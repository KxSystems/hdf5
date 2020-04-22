/* --- Read functionality
 * The following functions are used in conversion to q data from hdf5.
 * These functions assume that numeric data is being passed as a contiguous list "(raze/)data",
 * this may need to be reevaluated in order to improve efficiency.
*/


// Function prototypes for utility functions relating to reading of hdf5 data 
// these are defined later in this script
K readData(hid_t data, char *rdtyp);
K readCompound(hid_t dset, char *rdtyp);


// Read data from an attribute associated with a group or dataset
EXP K hdf5readAttrDataset(K fname, K dname, K aname){
  disable_err();
  // Type check inputs
  if(!checkType("[Cs][Cs][Cs]", fname, dname,aname))
    return KNL;
  // Define required elements
  K result;
  hid_t file, data, attr;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  // Open file and dataset
  file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  // error if file doesn't exist
  if(file < 0){
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"file does not exist");
  }
  data = isGroupData(file, dataname);
  if(data < 0){
    free(filename);
    free(dataname);
    free(attrname);
    H5Fclose(file);
    return krr((S)"dataset/group from which attribute is to be read to does not exist");
  }
  attr = H5Aopen(data, attrname, H5P_DEFAULT);
  if(attr < 0){
    closeGroupData(file,dataname,data);
    H5Fclose(file);
    free(filename);
    free(dataname);
    free(attrname);
    return krr((S)"This attribute does not exist");
  }
  result = readData(attr, "a");
  closeGroupData(file, dataname, data);
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Aclose(attr);
  return result;
}

// Read data from a signularly typed HDF5 dataset
EXP K hdf5readDataset(K fname, K dname){
  disable_err();
  // Type check inputs
  if(!checkType("[Cs][Cs]", fname, dname))
    return KNL;
  // Define required elements
  K result;
  hid_t file, data;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  // Open file and dataset
  file   = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  if(file < 0){
    free(filename);
    free(dataname);
    return krr((S)"file does not exist");
  }
  data   = H5Dopen(file, dataname, H5P_DEFAULT);
  if(data < 0){
    free(filename);
    free(dataname);
    H5Fclose(file);
    return krr((S)"dataset does not exist");
  }
  result = readData(data, "d");
  free(filename);
  free(dataname);
  H5Fclose(file);
  H5Dclose(data);
  return result;
}


// Read numerical data to kdb from a hdf5 object
K readData(hid_t data,char *rdtyp){
  hid_t dtype, ntype;
  K result;
  // Get type based on form of object being read
  if(strcmp("d",rdtyp)==0)
    dtype = H5Dget_type(data);
  else
    dtype = H5Aget_type(data);
  ntype = H5Tget_native_type(dtype,H5T_DIR_ASCEND);
  // Read as a compound datatype if suitable
  if(H5T_COMPOUND == H5Tget_class(dtype))
    result = readCompound(data,rdtyp);
  // Read vanilla datatypes
  else{
    if(H5Tequal(ntype, HDF5INT))
      result = readInt(data,rdtyp);
    else if(H5Tequal(ntype, HDF5LONG))
      result = readLong(data,rdtyp);
    else if(H5Tequal(ntype, HDF5FLOAT))
      result = readFloat(data,rdtyp);
    else if(H5Tequal(ntype, HDF5REAL))
      result = readReal(data,rdtyp);
    else if(H5Tequal(ntype, HDF5SHORT))
      result = readShort(data,rdtyp);
    else if(H5Tequal(ntype, H5T_NATIVE_USHORT))
      result = readUShort(data,rdtyp);
    else if(H5Tequal(ntype, H5T_NATIVE_UINT))
      result = readUInt(data,rdtyp);
    else if(H5Tequal(ntype, H5T_NATIVE_ULONG))
      result = readULong(data,rdtyp);
    else if(H5Tequal(ntype, H5T_NATIVE_B8))
      result = readBitfield(data,rdtyp);
    else if(H5Tequal(ntype, H5T_NATIVE_UCHAR))
      result = readByte(data,rdtyp);
    else 
      result = readChar(data, rdtyp);
  }
  H5Tclose(dtype);
  H5Tclose(ntype);
  return result;
}

// Recursively read data from a HDF5 compound dataset
K readCompound(hid_t dset, char *rdtyp){
  int i, nmembs, rank;
  hid_t dtype, space;
  hid_t ntype, item_type;
  H5T_class_t memb_cls;
  dtype  = H5Dget_type(dset);
  ntype  = H5Tget_native_type(dtype, H5T_DIR_ASCEND);
  K key_vals, data_vals, memb_val;
  data_vals = knk(0);
  if(strcmp(rdtyp,"a")==0)
    space  = H5Aget_space(dset);
  else
    space  = H5Dget_space(dset);
  rank   = H5Sget_simple_extent_ndims(space);
  hsize_t dims[rank];
  H5Sget_simple_extent_dims (space, dims, NULL);
  nmembs      = H5Tget_nmembers(ntype);
  key_vals    = ktn(KS, nmembs);
  for(i = 0;i < nmembs; i++){
    char* member_name = H5Tget_member_name(ntype,i);
    kS(key_vals)[i] = member_name;
    memb_cls  = H5Tget_member_class(ntype,i);
    item_type = H5Tget_member_type(dtype, i);
    if(H5Tequal(item_type,HDF5INT) || H5Tequal(item_type,HDF5LONG) || H5Tequal(item_type,HDF5SHORT))
      memb_val = compoundInteger(dset, member_name, dims[0], item_type,rdtyp);
    else if(H5Tequal(item_type,HDF5FLOAT) || H5Tequal(item_type,HDF5REAL))
      memb_val = compoundFloat(dset, member_name, dims[0], item_type, rdtyp);
    else if(H5Tequal(item_type,H5T_NATIVE_UCHAR) || H5Tequal(item_type,H5T_NATIVE_B8))
      memb_val = compoundByte(dset, member_name, dims[0], item_type, rdtyp);
    else if(H5Tequal(item_type,H5T_NATIVE_USHORT))
      memb_val = compoundUShort(dset, member_name, dims[0], item_type, rdtyp);
    else if(H5Tequal(item_type,H5T_NATIVE_UINT))
      memb_val = compoundUInt(dset, member_name, dims[0], item_type, rdtyp);
    else if(H5Tequal(item_type,H5T_NATIVE_ULONG))
      memb_val = compoundULong(dset, member_name, dims[0], item_type, rdtyp);
    else if(H5Tequal(item_type,H5T_NATIVE_HBOOL))
      memb_val = compoundBool(dset, member_name, dims[0], item_type, rdtyp);
    else if(memb_cls == H5T_STRING)
      memb_val = compoundString(dset, member_name, dims[0], item_type, rdtyp);
    jk(&data_vals,memb_val);
  }
  H5Tclose(dtype);
  H5Tclose(ntype);
  H5Tclose(item_type);
  H5Sclose(space);
  return(xT(xD(key_vals,data_vals)));
}

