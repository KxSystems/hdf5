// The following script shows that the interface can handle reading of HDF5 data compressed
// using another library in cases where this compression has been
// 1. szip; 2. gzip; 3. scale-offset; 4. n-bit
-1"Running h5_comp.q\n";
 
// import the hdf5 library
\l ../hdf5.q

// Move into the hdf5 namespace
\d .hdf5

szip_name:"example_data/h5ex_d_szip.h5"
gzip_name:"example_data/h5ex_d_gzip.h5"
soff_name:"example_data/h5ex_d_sofloat.h5"
nbit_name:"example_data/h5ex_d_nbit.h5"
dset_name:"DS1"

-1"Reading data compressed using szip";
show readData[szip_name;dset_name];
-1"\nReading data compressed using gzip";
show readData[gzip_name;dset_name];
-1"\nReading data compressed using Scale-offset compression";
show readData[soff_name;dset_name];
-1"\nReading data compressed using N-Bit compression";
show readData[nbit_name;dset_name];

// Clean up and exit
-1"\n---------------------------------------\n";
exit 0;
