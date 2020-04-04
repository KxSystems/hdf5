\d .hdf5
LIBPATH:`:hdf5 2:
funcs:(
  /* write functions */
  (`hdf5writeDataset;5);
  (`hdf5writeAttrDataset;6);

  /* create functions */
  (`hdf5createFile;1);
  (`hdf5createDataset;4);
  (`hdf5createAttr;5);
  
  /* General use functions */
  (`hdf5isAttr;3);
  (`hdf5ishdf5;1);
  (`hdf5isObject;2);
  (`hdf5fileSize;1);
  (`hdf5getAttrShape;3);
  (`hdf5getAttrPoints;3);
  (`hdf5getDataShape;2);
  (`hdf5getDataPoints;2);
  (`hdf5datasetInfo;2);
  (`hdf5version;1);
  (`hdf5copyObject;4);
  (`hdf5gc;1);
  (`hdf5ls;1);

  /* Deletion functions */
  (`hdf5delAttr;3);
  
  /* Read functionality */
  (`hdf5readDataset;2);
  (`hdf5readAttrDataset;3);
  
  /* Group function */
  (`hdf5createGroup;2);
  
  /* Linking functionality */
  (`hdf5createExternal;4);
  (`hdf5createHard;3);
  (`hdf5createSoft;3);
  (`hdf5delLink;2)
  )

// Add all functions read from C to the hdf5 namespace
.hdf5,:(`$4_'string funcs[;0])!LIBPATH@/:funcs

// Find the appropriate type for a dataset being written to hdf5
i.self_type:{$[any 0h in type each x;.z.s each x;raze type each x]}
i.nlist_types:{$[1=count distinct l:i.self_type x;(abs distinct raze l)0;'"mixed list detected"]}
i.fntyp:{first(.Q.t til 20)@i.nlist_types[x]}

writeData:{[fname;dname;dset]
  if[11h = abs type dset;dset:string dset];
  if[10h = abs type dset;dset:enlist dset];
  typ:i.fntyp dset;
  dims:"i"$i.shape dset;
  dset:$[typ in "cs";$[typ = "s";string dset;dset];$[typ="b";"i"$dset;dset]];
  writeDataset[fname;dname;dset;dims;typ]
  }

writeAttr:{[fname;dname;aname;dset]
  if[11h = abs type dset;dset:string dset];
  if[10h = abs type dset;dset:enlist dset];
  typ:i.fntyp dset;
  dims:"i"$i.shape dset;
  dset:$[typ in "cs";$[typ = "s";string dset;dset];dset];
  writeAttrDataset[fname;dname;aname;dset;dims;typ]
  }

readAttr:{[fname;dname;aname]getAttrShape[fname;dname;aname]#readAttrDataset[fname;dname;aname]}
readData:{[fname;dname]getDataShape[fname;dname]#readDataset[fname;dname]}

i.shape:{-1_count each first scan x}
