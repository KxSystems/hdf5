\d .hdf5
LIBPATH:`:libhdf5 2:
funcs:(
  // initialize
     (`hdf5init;1);
         // .hdf5.init[]_
  // create
     (`hdf5createFile;1);
         // .hdf5.createFile[fname:Cs]:_
     (`hdf5createGroup;2);
         // .hdf5.createGroup[fname:Cs; gname:Cs]:_
     (`hdf5createDataset;4);
         // .hdf5.createDataset[fname:Cs; dname:Cs; kdims:I; ktype:c]:_
     (`hdf5createAttr;5);
         // .hdf5.createAttr[fname:Cs; dname:Cs; aname:Cs; kdims:I; ktype:c]:_
  // delete
     (`hdf5delAttr;3);
         // .hdf5.delAttr[fname:Cs; dname:Cs; aname:Cs]:_
  // read
     (`hdf5readDataset;2);
         // .hdf5.readDataset[fname:Cs; dname:Cs]:*
     (`hdf5readAttrDataset;3);
         // .hdf5.readAttrDataset[fname:Cs; dname:Cs; aname:Cs]:*
  // write
     (`hdf5writeDataset;5);
         // .hdf5.writeDataset[fname:CS; dname:Cs; dset:*; kdims:I; ktype:c]:_
     (`hdf5writeAttrDataset;6);
         // .hdf5.writeAttrDataset[fname:Cs; dname:Cs; aname:Cs; dset:*; kdims:I; ktype:c]:_
  // general-use functions
     (`hdf5version;1);
         // .hdf5.version[]:S!J
     (`hdf5errorOn;1);
         // .hdf5.errorOn[]:_
     (`hdf5errorOff;1);
         // .hdf5.errorOff[]:_
     (`hdf5ishdf5;1);
         // .hdf5.ishdf5[fname:Cs]:b
     (`hdf5isObject;2);
         // .hdf5.isObject[fname:Cs; oname:Cs]:b
     (`hdf5isAttr;3);
         // .hdf5.isAttr[fname:Cs; dname:Cs; aname:Cs]:b
     (`hdf5fileSize;1);
         // .hdf5.fileSize[fname:Cs]:f
     (`hdf5dataSize;2);
         // .hdf5.dataSize[fname:Cs; dname:Cs]:f
     (`hdf5getDataShape;2);
         // .hdf5.getDataShape[fname:Cs; dname:Cs]:J
     (`hdf5getDataPoints;2);
         // .hdf5.getDataPoints[fname:Cs; dname:Cs]:j
     (`hdf5getAttrShape;3);
         // .hdf5.getAttrShape[fname:Cs; dname:Cs; aname:Cs]:J
     (`hdf5getAttrPoints;3);
         // .hdf5.getAttrPoints[fname:Cs; dname:Cs; aname:Cs]:j
     (`hdf5datasetType;2);
         // .hdf5.datasetType[fname:Cs; dname:Cs]:s
     (`hdf5datasetInfo;2);
         // .hdf5.datasetInfo[fname:Cs; dname:Cs]:S!*
     (`hdf5copyObject;4);
         // .hdf5.copyObject[srcfile:CS; src_obj:Cs; dstfile:Cs; dst_obj:Cs]:_
     (`hdf5ls;1);
         // .hdf5.ls[fname:Cs]:_
     (`hdf5gc;1);
         // .hdf5.gc[]:i
  // links
     (`hdf5createExternal;4);
         // .hdf5.createExternal[fname:Cs; lname:Cs; tname:Cs; tobj:Cs]:_
     (`hdf5createHard;3);
         // .hdf5.createHard[fname:Cs; oname:Cs; lname:Cs]:_
     (`hdf5createSoft;3);
         // .hdf5.createSoft[fname:Cs; tpath:Cs; lname:Cs]:_
     (`hdf5delLink;2)
         // .hdf5.delLink[fname:Cs; lname:Cs]:_
 );
// binding functions from c to q
// hdf5<Name> -> .hdf5.<Name>
.hdf5,:(`$4_'string funcs[;0])!LIBPATH@/:funcs

i.ktypes:reverse[1_20#.Q.t],upper 20#.Q.t
i.checkdata:{
  r:{$[0=type x;count[x],'distinct raze .z.s each x;10h=type x;10h;enlist(count x;neg type x)]}x;
  if[1<count r;'`$"invalid ",$[1<count distinct last each r;"type";"shape"]];
  if[10h~r:first r ;r:count[x],-10h];
  `kdims`ktype`flatdata!(-1_r;i.ktypes 19+last r;(count[r]-2)raze/x)}

// Find the appropriate type for a dataset being written to hdf5
i.self_type:{$[any 0h in type each x;.z.s each x;raze type each x]}
i.nlist_types:{$[1=count distinct l:i.self_type x;(abs distinct raze l)0;'"mixed list detected"]}
i.fntyp:{first(.Q.t til 20)@i.nlist_types[x]}

datamap:`b`p`m`d`z`n`u`v`t!`boolean`timestamp`month`date`datetime`timespan`minute`second`time;

writeData:{[fname;dname;dset]
  if[type[dset]in 98 99h;:writeDictTab[fname;dname;dset]];
  if[11h = abs type dset;dset:string dset];
  dset:$[10h=type dset;
         enlist dset;
         -10h=type dset;
         enlist enlist dset;
         dset];
  typ:i.fntyp dset;
  dims:"i"$i.shape dset;
  dset:$[typ in "csg";
         $[typ in "gs";string dset;dset];
         $[typ in "bmduvt";"i"$dset;
           typ in "pn";"j"$dset;
           typ = "z";"f"$dset;
           dset]];
  writeDataset[fname;dname;dset;dims;typ];
  if[typ in"bpmdznuvt";writeAttr[fname;dname;"datatype_kdb";datamap`$typ]]
  }

writeAttr:{[fname;dname;aname;dset]
  if[11h = abs type dset;dset:string dset];
  dset:$[10h=type dset;
         enlist dset;
         -10h=type dset;
         enlist enlist dset;
         dset];
  typ:i.fntyp dset;
  dims:"i"$i.shape dset;
  dset:$[typ in "csg";
         $[typ in "gs";string dset;dset];
         $[typ in "bmduvt";"i"$dset;
           typ in "pn";"j"$dset;
           typ = "z";"f"$dset;
           dset]];
  if[typ in" ";'"This data format cannot be written to an attribute"];
  writeAttrDataset[fname;dname;aname;dset;dims;typ]
  }

readAttr:{[fname;dname;aname]getAttrShape[fname;dname;aname]#readAttrDataset[fname;dname;aname]}
readData:{[fname;dname]
  if[isAttr[fname;dname;"datatype_kdb"];
    typ:readAttr[fname;dname;"datatype_kdb"]0;
    if[(typ~"table")|typ~"dict";:readDictTab[fname;dname;typ]]];
  data:getDataShape[fname;dname]#readDataset[fname;dname];
  $[isAttr[fname;dname;"datatype_kdb"];(first `$readAttr[fname;dname;"datatype_kdb"])$;]data}

i.shape:{-1_count each first scan x}

// Functionality to recursively write a kdb+ table/dictionary to a hdf5 group
writeDictTab:{[fname;dname;dset]
  createGroup[fname;dname];
  $[98h=type dset;
    [writeAttr[fname;dname;"datatype_kdb";"table"];
     writeAttr[fname;dname;"kdb_columns";cols[dset]];
     dict_keys:dname,/:"/",/:string cols dset;
     data:value flip dset]; 
    [writeAttr[fname;dname;"datatype_kdb";"dict"]
    writeAttr[fname;dname;"kdb_keys";kvals:key[dset]];
    dict_keys:dname,/:"/",/:string kvals;
    data:value dset]];
  writeData[fname]'[dict_keys;data];
  }

// Read a kdb+ table/dictionary written from q to a group
readDictTab:{[fname;dname;typ]
  $["table"~typ;
    [tab_cols:readAttr[fname;dname;"kdb_columns"];
     kdb_data:readData[fname]each dname,/:"/",/:tab_cols;
     r:flip (`$tab_cols)!kdb_data];
    [dict_cols:readAttr[fname;dname;"kdb_keys"];
     kdb_data:readData[fname]each dname,/:"/",/:dict_cols;
     r:(`$dict_cols)!kdb_data]
   ];
  r
  }

// initialize
init[];
