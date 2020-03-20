// This will create a group or set of intermediate groups /group1 ... /group1/group11/group21 
EXP K hdf5createGroup(K fname, K gname){
  disable_err();
  if(!checkType("[Cs][Cs]", fname, gname))
    return KNL;
  // gcpl -> group creation property list
  hid_t file, group, gcpl;
  char *filename   = getkstring(fname);
  char *groupnames = getkstring(gname);
  // Create a file is it does not exist
  htri_t file_nm = ish5(filename);
  if(file_nm < 0)
    createfile(filename);
  if(file_nm == 0){
    free(groupnames);
    free(filename);
    return krr((S)"This file already exists and is not a hdf5 file");
  }
  file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
  gcpl = H5Pcreate(H5P_LINK_CREATE);
  // 2nd arg -> 1 = create any missing groups
  H5Pset_create_intermediate_group(gcpl, 1);
  group = H5Gcreate(file, groupnames, gcpl, H5P_DEFAULT, H5P_DEFAULT);
  free(filename);
  free(groupnames);
  H5Pclose(gcpl);
  H5Gclose(group);
  H5Fclose(file);
  return 0;
}
