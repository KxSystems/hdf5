\l hdf5.q

E:"test_real.h5"
F:"test_float.h5"
H:"test_short.h5"
I:"test_int.h5"
B:"test_bool.h5"
J:"test_long.h5"
C:"test_char.h5"
S:"test_symbol.h5"
dset:"/dset"

.hdf5.createFile[E]
.hdf5.createFile[F]
.hdf5.createFile[H]
.hdf5.createFile[I]
.hdf5.createFile[B]
.hdf5.createFile[J]
.hdf5.createFile[C]
.hdf5.createFile[S]


// Testing dataset functionality

N:100;M:100;K:100
// 1-Dimensional dataset
data_real_1 :enlist N?10e
data_float_1:enlist N?10f
data_short_1:enlist N?10h
data_int_1  :enlist N?10i
data_bool_1 :enlist N?0b
data_long_1 :enlist N?10

// Create arrays containing symbols and chars
data_char_arr:N?("test";"testing";"array")
data_sym_arr:N?`4

// 2-Dimensional dataset
data_real_2 :(N,M)#(N*M)?10e
data_float_2:(N,M)#(N*M)?10f
data_short_2:(N,M)#(N*M)?10h
data_int_2  :(N,M)#(N*M)?10i
data_bool_2 :(N,M)#(N*M)?0b
data_long_2 :(N,M)#(N*M)?10

// 3-Dimensional dataset
data_real_3 :(N,M,K)#(N*M*K)?10e
data_float_3:(N,M,K)#(N*M*K)?10f
data_short_3:(N,M,K)#(N*M*K)?10h
data_int_3  :(N,M,K)#(N*M*K)?10i
data_bool_3 :(N,M,K)#(N*M*K)?0b
data_long_3 :(N,M,K)#(N*M*K)?10

// Create a dataset at the root of the file 
.hdf5.createDataset[E;dset,"_1";enlist `int$N;"e"]
.hdf5.createDataset[F;dset,"_1";enlist `int$N;"f"]
.hdf5.createDataset[H;dset,"_1";enlist `int$N;"h"]
.hdf5.createDataset[I;dset,"_1";enlist `int$N;"i"]
.hdf5.createDataset[B;dset,"_1";enlist `int$N;"b"]
.hdf5.createDataset[J;dset,"_1";enlist `int$N;"j"]
.hdf5.createDataset[C;dset,"_1";enlist `int$N;"c"]

.hdf5.createDataset[E;dset,"_2";`int$(N;M);"e"]
.hdf5.createDataset[F;dset,"_2";`int$(N;M);"f"]
.hdf5.createDataset[H;dset,"_2";`int$(N;M);"h"]
.hdf5.createDataset[I;dset,"_2";`int$(N;M);"i"]
.hdf5.createDataset[B;dset,"_2";`int$(N;M);"b"]
.hdf5.createDataset[J;dset,"_2";`int$(N;M);"j"]

// Write to the non grouped dataset at file root
.hdf5.writeData[E;dset,"_1";data_real_1]
.hdf5.writeData[F;dset,"_1";data_float_1]
.hdf5.writeData[H;dset,"_1";data_short_1]
.hdf5.writeData[I;dset,"_1";data_int_1]
.hdf5.writeData[B;dset,"_1";data_bool_1]
.hdf5.writeData[J;dset,"_1";data_long_1]
.hdf5.writeData[C;dset,"_1";data_char_arr]
.hdf5.writeData[S;dset,"_1";data_sym_arr]

.hdf5.writeData[E;dset,"_2";data_real_2]
.hdf5.writeData[F;dset,"_2";data_float_2]
.hdf5.writeData[H;dset,"_2";data_short_2]
.hdf5.writeData[I;dset,"_2";data_int_2]
.hdf5.writeData[B;dset,"_2";data_bool_2]
.hdf5.writeData[J;dset,"_2";data_long_2]

.hdf5.writeData[E;dset,"_3";data_real_3]
.hdf5.writeData[F;dset,"_3";data_float_3]
.hdf5.writeData[H;dset,"_3";data_short_3]
.hdf5.writeData[I;dset,"_3";data_int_3]
.hdf5.writeData[B;dset,"_3";data_bool_3]
.hdf5.writeData[J;dset,"_3";data_long_3]

first[data_real_1]  ~ .hdf5.readData[E;dset,"_1"]
first[data_float_1] ~ .hdf5.readData[F;dset,"_1"]
first[data_short_1] ~ .hdf5.readData[H;dset,"_1"]
first[data_int_1]   ~ .hdf5.readData[I;dset,"_1"]
first[data_bool_1]  ~ "b"$.hdf5.readData[B;dset,"_1"]
first[data_long_1]  ~ .hdf5.readData[J;dset,"_1"]
data_char_arr       ~ .hdf5.readData[C;dset,"_1"]
data_sym_arr        ~ `$.hdf5.readData[S;dset,"_1"]

data_real_2  ~ .hdf5.readData[E;dset,"_2"]
data_float_2 ~ .hdf5.readData[F;dset,"_2"]
data_short_2 ~ .hdf5.readData[H;dset,"_2"]
data_int_2   ~ .hdf5.readData[I;dset,"_2"]
data_bool_2  ~ "b"$.hdf5.readData[B;dset,"_2"]
data_long_2  ~ .hdf5.readData[J;dset,"_2"]

data_real_3  ~ .hdf5.readData[E;dset,"_3"]
data_float_3 ~ .hdf5.readData[F;dset,"_3"]
data_short_3 ~ .hdf5.readData[H;dset,"_3"]
data_int_3   ~ .hdf5.readData[I;dset,"_3"]
data_bool_3  ~ "b"$.hdf5.readData[B;dset,"_3"]
data_long_3  ~ .hdf5.readData[J;dset,"_3"]


// Testing grouping functionality

// Create a group "/Group1" within the previously created files
G1:"/Group1"
.hdf5.createGroup[E;G1]
.hdf5.createGroup[F;G1]
.hdf5.createGroup[H;G1]
.hdf5.createGroup[I;G1]
.hdf5.createGroup[B;G1]
.hdf5.createGroup[J;G1]

// Create a dataset to be populated within the groups
G1dset:G1,dset
.hdf5.createDataset[E;G1dset;`int$(N;M);"e"]
.hdf5.createDataset[F;G1dset;`int$(N;M);"f"]
.hdf5.createDataset[H;G1dset;`int$(N;M);"h"]
.hdf5.createDataset[I;G1dset;`int$(N;M);"i"]
.hdf5.createDataset[B;G1dset;`int$(N;M);"i"]
.hdf5.createDataset[J;G1dset;`int$(N;M);"j"]

// Write to the datasets within groups
.hdf5.writeData[E;G1dset;data_real_2]
.hdf5.writeData[F;G1dset;data_float_2]
.hdf5.writeData[H;G1dset;data_short_2]
.hdf5.writeData[I;G1dset;data_int_2]
.hdf5.writeData[B;G1dset;data_bool_2]
.hdf5.writeData[J;G1dset;data_long_2]

data_real_2  ~ .hdf5.readData[E;G1dset]
data_float_2 ~ .hdf5.readData[F;G1dset]
data_short_2 ~ .hdf5.readData[H;G1dset]
data_int_2   ~ .hdf5.readData[I;G1dset]
data_bool_2  ~ "b"$.hdf5.readData[B;G1dset]
data_long_2  ~ .hdf5.readData[J;G1dset]

// Testing attribute functionality
// These tests will need to be expanded moving forward to incorporate checking of more explicit types
attrdata:2 2#4?10i;
.hdf5.createAttr[E;dset,"_2";"int attribute";2 2i;"i"]
.hdf5.isAttr[E;dset,"_2";"int attribute"]
.hdf5.writeAttr[E;dset,"_2";"int attribute";attrdata]
attrdata ~ .hdf5.readAttr[E;dset,"_2";"int attribute"]


// Testing utility functions

// Create a file to check function of ishdf5
`:test_txt.txt 0: enlist "text to save";

not .hdf5.ishdf5["test_txt.txt"]
.hdf5.ishdf5["test_float.h5"]

.hdf5.getDataShape[E;G1dset]     ~ .hdf5.i.shape[data_real_2]
.hdf5.getDataPoints[F;dset,"_3"] ~ (count (raze/)data_float_3) 

.hdf5.datasetInfo[J;dset,"_2"]~`type`ndims`dims!(`long;2i;100 100i)
.hdf5.datasetInfo[F;dset,"_3"]~`type`ndims`dims!(`float;3i;100 100 100i)

// Clean up
$[.z.o like "w*";system "del test_*";system"rm test_*"];

