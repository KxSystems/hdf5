// Create a hdf5 dataset containing example experimental data. 
// Assign two attributes one with unit type for the data and one with
// information on the 'temperatures' of the room.
// Read the attributes from the datasets back to kdb

-1"Running h5_attr.q\n";
 
// import the hdf5 library
\l ../q/hdf5.q

// Move into the hdf5 namespace
\d .hdf5

// Initialize the file
fname:"test.h5";
createFile[fname];

// Inititalize the name and values of the datasets
dname:"dset";
dset :2 100#200?1f;

// Initialize the names and values of the attributes 
attr1_name:"Units";
attr2_name:"Temperatures at sensors";
attr1_val :"Voltage/s"
attr2_val :21+2?1f

-1"Write the dataset '",dname,"' to the file '",fname,"'\n";
writeData[fname;dname;dset];

-1"Write the attributes '",attr1_name,"' and '",attr2_name,"' to the dataset '",dname,"'\n";
writeAttr[fname;dname;attr1_name;attr1_val];
writeAttr[fname;dname;attr2_name;attr2_val];

-1"Check if the attributes exist";
-1"Does '",attr1_name,"' exist?";
show isAttr[fname;dname;attr1_name];
-1"Does '",attr2_name,"' exist?";
show isAttr[fname;dname;attr2_name];

-1"The value of '",attr1_name,"' is = ",readAttr[fname;dname;attr1_name]0;
-1"The value of '",attr2_name,"' is = ",sv[", ";string[readAttr[fname;dname;attr2_name]]],"\n";

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;
