// Create a dataset within a h5 file. Write to the dataset.
// Read the dataset back to kdb+ and check equality of the two
  
-1"Running h5_dset.q\n";

// import the hdf5 library
\l ../q/hdf5.q

// Move into the hdf5 namespace
\d .hdf5

// Name the file to which the dataset is to be added
fname:"test.h5";
// Inititalize the name of the dataset
dname:"dset";

-1"The dataset '",dname,"' will be written to the file '",fname,"'\n";
-1"The following is the dataset to be written to file:\n";
show dset:3 100000#300000?1f;

-1"\nWriting the dataset to file\n";
createFile[fname];
writeData[fname;dname;dset];
-1"Reading the dataset back to kdb+\n";
show rdset:readData[fname;dname];

-1"\nAre the datasets equivalent?";
show rdset~dset;

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;
