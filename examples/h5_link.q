// Create a h5 file containing a dataset.
// Check file size as a benchmark.
// Create a soft link between the dataset and another location.
// Show the increase in file size and that the dataset accessed from different paths are the same
// Copy the initial dataset to another location in the file
// Show the change in file size and that these datasets are also equivalent

-1"Running h5_link.q\n";

// import the hdf5 library
\l ../hdf5.q

// Move into the hdf5 namespace
\d .hdf5

// Initializing file naming and datasets
fname:"test.h5"
init_dname:"initial_dset"
init_dset:10 100 1000#1000000?10h
-1"The dataset '",init_dname,"' will be written to the file ",fname,"\n";

-1"Writing 10 x 100 x 1000 short dataset to file\n";
writeData[fname;init_dname;init_dset];

// Initial file size
init_sz:fileSize[fname];
-1"The size of the file containing only the initial dataset is ",string[init_sz],"MB\n";

// Name of the soft link
slink:"soft_link_dset";
-1"Creating a symbolic link '",slink,"' from which to access '",init_dname,"\n";
createSoft[fname;init_dname;slink];

slink_sz:fileSize[fname];

-1"The file size has increased by ",string[slink_sz-init_sz],"MB with the additional link\n";

-1"Are the datasets accessed using the two paths equivalent?";
show readData[fname;init_dname] ~ readData[fname;slink];

// Name of the copied dataset
cpdset:"copied_dset";
-1"\nCreating a copy of '",init_dname,"' named '",cpdset,"' within the file '",fname,"'\n";
copyObject[fname;init_dname;fname;cpdset];

-1"The addition of the copied dataset increased the file size by ",
   string[fileSize[fname]-slink_sz],"MB\n";

-1"Are the copied and initial datasets equivalent?";
show readData[fname;init_dname] ~ readData[fname;cpdset];

-1"The following is the structure of the file following creation of the copied and symbolic links\n";
ls[fname];
-1"";

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;

