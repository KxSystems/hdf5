// Create a set of groups within a h5 file.
// Initially create the first group without the file existing
// following this add sub groups within the initial file
// display the file structure

-1"Running h5_group.q\n";
  
// import the hdf5 library
\l ../q/hdf5.q

// Move into the hdf5 namespace
\d .hdf5

// Name the file to which the group is to be added
fname:"test.h5";
// Inititalize the name of the first group
gname1:"Group1";

-1"Creating a file '",fname,"' containing an initial group '",gname1,"'\n";
createFile[fname];
createGroup[fname;gname1];

-1"The following is the initial structure of the file:\n";
show ls[fname];

// Initialize subgroups
gname2:"Group1/SG1"
gname3:"Group1/SG2/SG3"

-1"\nAdding subgroups '",gname2,"' and '",gname3,"' to the file\n";
createGroup[fname;gname2];
createGroup[fname;gname3];

-1"The following is the file structure following the addition of sub groups\n";
ls[fname];
-1"";

// Clean up and exit
$[.z.o like "w*";system "del test.h5";system"rm test.h5"];
-1"---------------------------------------\n";
exit 0;
