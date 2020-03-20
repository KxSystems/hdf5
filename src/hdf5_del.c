/* --- Deletion functionality --- */

EXP K hdf5delAttr(K fname, K dname, K aname){
  if(!checkType("[Cs][Cs][Cs]", fname, dname, aname))
    return KNL;
  K res;
  hid_t file, data;
  herr_t adel;
  char *filename = getkstring(fname);
  char *dataname = getkstring(dname);
  char *attrname = getkstring(aname);
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  // Is the object that you are deleting from an attribute or dataset
  data = isGroupData(file,dataname);
  adel = H5Adelete(data,attrname);
  // Indicate if deletion has been
  if(adel >= 0)
    res = kp("Successfully deleted attribute");
  else
    res = kp("Attribute not deleted/error");
  // Clean up
  closeGroupData(file, dataname, data);
  H5Fclose(file);
  free(filename);
  free(dataname);
  free(attrname);
  return res;
}
