// Create a h5 file using the hdf5-kdb interface

-1"\nRunning h5_file.q\n";

// import the hdf5 library
\l ../hdf5.q

// Move into the hdf5 namespace
\d .hdf5

// Name the file to be created
fname:"test.h5"

// check if a file exists, if it's already a hdf5 file highlight to the user
$[ishdf5[fname];
  [-1"file already exists, it will not be created\n";];
  [createFile[fname];-1"The hdf5 file ",fname," was created\n";]]

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;
