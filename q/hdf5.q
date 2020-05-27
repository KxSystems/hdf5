\d .hdf5
LIBPATH:`:libkdbhdf5 2:
funcs:(
     (`hdf5init;1);
         // .hdf5.init[]_
     (`hdf5createFile;1);
         // .hdf5.createFile[fname:C]:_
     (`hdf5createGroup;2);
         // .hdf5.createGroup[fname:C; gname:C]:_
     (`hdf5createDataset;4);
         // .hdf5.createDataset[fname:C; dname:C; kdims:J; ktype:c]:_
     (`hdf5createAttr;5);
         // .hdf5.createAttr[fname:C; dname:C; aname:C; kdims:J; ktype:c]:_
     (`hdf5delAttr;3);
         // .hdf5.delAttr[fname:Cs; dname:Cs; aname:Cs]:_
     (`hdf5readDataset;2);
         // .hdf5.readDataset[fname:C; dname:C]:*
     (`hdf5readAttrDataset;3);
         // .hdf5.readAttrDataset[fname:C; dname:C; aname:C]:*
     (`hdf5writeDataset;5);
         // .hdf5.writeDataset[fname:C; dname:C; dset:*; kdims:J; ktype:c]:_
     (`hdf5writeAttrDataset;6);
         // .hdf5.writeAttrDataset[fname:C; dname:C; aname:C; dset:*; kdims:J; ktype:c]:_
     (`hdf5version;1);
         // .hdf5.version[]:S!J
     (`hdf5errorOn;1);
         // .hdf5.errorOn[]:_
     (`hdf5errorOff;1);
         // .hdf5.errorOff[]:_
     (`hdf5ishdf5;1);
         // .hdf5.ishdf5[fname:C]:b
     (`hdf5isObject;2);
         // .hdf5.isObject[fname:C; oname:C]:b
     (`hdf5isAttr;3);
         // .hdf5.isAttr[fname:C; dname:C; aname:C]:b
     (`hdf5fileSize;1);
         // .hdf5.fileSize[fname:C]:f
     (`hdf5dataSize;2);
         // .hdf5.dataSize[fname:C; dname:C]:f
     (`hdf5getDataShape;2);
         // .hdf5.getDataShape[fname:C; dname:C]:J
     (`hdf5getAttrShape;3);
         // .hdf5.getAttrShape[fname:C; dname:C; aname:C]:J
     (`hdf5copyObject;4);
         // .hdf5.copyObject[srcfile:C; src_obj:C; dstfile:C; dst_obj:C]:_
     (`hdf5ls;1);
         // .hdf5.ls[fname:C]:_
     (`hdf5gc;1);
         // .hdf5.gc[]:i
     (`hdf5createExternal;4);
         // .hdf5.createExternal[linkfile:C; linkpath:C; targetfile:C; targetpath:C]:_
     (`hdf5createHard;3);
         // .hdf5.createHard[linkfile:C; linkpath:C; targetpath:C]:_
     (`hdf5createSoft;3);
         // .hdf5.createSoft[linkfile:C; linkpath:C; targetpath:C]:_
     (`hdf5delLink;2)
         // .hdf5.delLink[linkfile:C; linkpath:C]:_
 );

// binding functions from c to q
// hdf5<Name> -> .hdf5.<Name>
.hdf5,:(`$4_'string funcs[;0])!LIBPATH@/:funcs

i.typeConv:{$[x in"sg";string;("*"^("bmduvtpnz"!"xiiiiijjf")x)$]}
i.typeRead:{$[x="s";`;x="g";"G";x]$}
i.typeChar:{$[0>x;::;upper].Q.t abs x}
i.checkDimsType:{$[0=t:type x;count[x],'distinct raze .z.s each x;10=t;t;enlist(count x;neg t)]}
i.checkData:{
  if[1<count dt:i.checkDimsType x;'"invalid ",$[1<count distinct last each dt;"type";"shape"]];
  if[10h~dt:first dt;dt:count[x],-10h];
  kdims:-1_dt;
  ktype:i.typeChar last dt;
  kdata:i.typeConv[ktype](count[kdims]-1)raze/x;
  `kdims`ktype`kdata!(kdims;ktype;kdata)}

writeData:{[fname;dname;dset]
  if[type[dset]in 98 99h;:writeDictTab[fname;dname;dset]];
  chk:i.checkData dset;
  createDataset[fname;dname] . chk`kdims`ktype;
  writeDataset[fname;dname] . chk`kdata`kdims`ktype;
  if[chk[`ktype]in"bmduvtpnzsg";
    writeAttr[fname;dname;"datatype_kdb"]enlist chk`ktype];
  }
writeAttr:{[fname;dname;aname;dset]
  if[type[dset]in 98 99h;:writeDictTab[fname;dname;dset]];
  chk:i.checkData dset;
  if[not chk[`ktype]in "xihjfecCs";'"kdb+ type can not be mapped to an appropriate attribute"];
  createAttr[fname;dname;aname] . chk`kdims`ktype;
  writeAttrDataset[fname;dname;aname] . chk`kdata`kdims`ktype;
  }
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

readData:{[fname;dname]
  typ:$[isAttr[fname;dname;"datatype_kdb"];readAttr[fname;dname;"datatype_kdb"];"*"];
  if[(typ~"table")|typ~"dict";:readDictTab[fname;dname;typ]];
  i.typeRead[first typ]getDataShape[fname;dname]#readDataset[fname;dname]
  }
readAttr:{[fname;dname;aname]
  getAttrShape[fname;dname;aname]#readAttrDataset[fname;dname;aname]
  }
readDictTab:{[fname;dname;typ]
  $["table"~typ;
    [tab_cols:readAttr[fname;dname;"kdb_columns"];
     kdb_data:readData[fname]each dname,/:"/",/:tab_cols;
     r:flip (`$tab_cols)!kdb_data];
    [dict_cols:readAttr[fname;dname;"kdb_keys"];
     kdb_data:readData[fname]each dname,/:"/",/:dict_cols;
     r:(`$dict_cols)!kdb_data]
  ];r}

// initialize
init[];
