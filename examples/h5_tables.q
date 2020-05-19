// The following functionality shows how HDF5 compound datatypes are read from HDF5 
// and how simple kdb+ tables and dictionaries are written to and read from a HDF5 format.
// A sample hdf5 style compound table is located in example_data/table.h5

-1"Running h5_tables.q\n";
 
// import the hdf5 library
\l ../q/hdf5.q

// Move into the hdf5 namespace
\d .hdf5

data_name:"example_data/table.h5"

-1"What is the structure of '",data_name,"' which contain a standard HDF5 compound dataset?";
ls[data_name];

-1"\nRead the compound dataset table from '",data_name,"'";
show test_data:readData[data_name;"table"];

// Naming of the file and kdb table to be written to HDF5
fname:"test.h5";
createFile[fname];
write_name:"kdb_table";

-1"\nWrite the above table to HDF5 using the kdb+ interface method ",
  "naming the table '",write_name,"' to the new file '",fname,"'\n";
writeData[fname;write_name;test_data];
-1"What does the this data look like saved to '",fname,"'?\n";
ls[fname];

-1"\nDisplay the attribute 'datatype_kdb' associated with the ",
  "group 'kdb_table' and read table back 'kdb_table'\n";
-1"The group '",write_name,"' has and attribute 'datatype_kdb' = '",
  (readAttr[fname;write_name;"datatype_kdb"]0),"'\n";
show readData[fname;write_name];
-1"";

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;
