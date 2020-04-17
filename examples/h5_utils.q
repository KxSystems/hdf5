// Examples of various utility functions available within the interface

-1"Running h5_utils.q\n";
 
// import the hdf5 library
\l ../hdf5.q

// Move into the hdf5 namespace
\d .hdf5

-1"What is the version of the HDF5 C api I am currently using?";
show version[];

-1"\nRun HDF5 garbage collection on the dataset and return the reclaimed memory";
show gc[];

-1"\nCreate a dataset named 'dset' in the file 'test.h5' and return information about the dataset";
writeData["test.h5";"dset";202 10#2020?1e];
show datasetInfo["test.h5";"dset"];
-1"\nHow many data points make up the dataset?";
show getDataPoints["test.h5";"dset"];
-1"\nWhat memory in MB is required to store this dataset?";
show dataSize["test.h5";"dset"];

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"\n---------------------------------------\n";
exit 0;
