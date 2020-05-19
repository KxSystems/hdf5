\l q/hdf5.q

fname:"test_file.h5"
attr_dset:"attr_dset";
.hdf5.createFile[fname];

gen_data:{@[;0;string]x#/:prd[x]?/:(`6;`6;0Ng;.Q.a),("bxhijefpdznuvt"$\:0)}
gen_names:{"dset_",/:x,/:string til count y}

dset_1D:gen_data[enlist 50];
dset_3D:gen_data[10 5 2];
dset_5D:gen_data[5 4 3 2 1];

one_tb  :("dset_1_tab";flip(`$gen_names["1_tab";dset_1D])!dset_1D);
three_tb:("dset_3_tab";flip(`$gen_names["3_tab";dset_3D])!dset_3D);
five_tb :("dset_5_tab";flip(`$gen_names["5_tab";dset_5D])!dset_5D);

test_dset:{.hdf5.writeData[x;y;z];z~.hdf5.readData[x;y]}[fname]
test_attr:{[x;y;z;k].hdf5.writeAttr[x;y;z;k];k~.hdf5.readAttr[x;y;z]}[fname;attr_dset]
test_fail:{0b~$[(::)~ .[x;y;{[x]x;0b}];1b;0b]}

-1"Testing numeric types";
one_d  :flip(gen_names["1_";dset_3D];dset_1D);
three_d:flip(gen_names["3_";dset_3D];dset_3D);
five_d :flip(gen_names["5_";dset_5D];dset_5D);
(all/){test_dset .'x}each(one_d;three_d;five_d)
-1"Testing numeric types complete\n";

-1"Testing table and dictionary write/read";
one_tb  :("dset_1_tab";flip(`$gen_names["1_tab";dset_1D])!dset_1D);
three_tb:("dset_3_tab";flip(`$gen_names["3_tab";dset_3D])!dset_3D);
five_tb :("dset_5_tab";flip(`$gen_names["5_tab";dset_5D])!dset_5D);
(all/){test_dset . x}each(one_tb;three_tb;five_tb)
-1"Testing table and dictionary write/read complete\n";

-1"Testing writing data to Groups";
gname1:"Group1/";
gname2:"Group1/Group2/";
.hdf5.createGroup[fname;gname1]
.hdf5.createGroup[fname;gname2]
gname1_d:flip(gname1,/:gen_names["1_";dset_1D];dset_1D);
gname2_d:flip(gname2,/:gen_names["3_";dset_3D];dset_3D);
(all/){test_dset .'x}each(gname1_d;gname2_d)
-1"Testing writing data to Groups complete\n";

-1"Testing attribute functions";
attr1:"int attr";
attr2:"string attr";
num_tst:2 2#4?5i;
str_tst:"Testing string input";
.hdf5.createDataset[fname;attr_dset;4 5;"f"];
.hdf5.createAttr[fname;attr_dset;attr1;2 2;"i"];
.hdf5.isAttr[fname;attr_dset;attr1]
not .hdf5.isAttr[fname;attr_dset;attr2]
.hdf5.writeAttr[fname;attr_dset;"str attr";str_tst];
.hdf5.writeAttr[fname;attr_dset;"num attr";num_tst];
// Generate all data types which will fail for writeAttr
attr_gen:{x#/:prd[x]?/:0Ng,("bpdznuvt"$\:0)};
attr_tst_data:attr_gen[100 20];
attr_names:enlist each gen_names["attr_";attr_tst_data];
attr_data:raze each(enlist fname;enlist attr_dset),/:enlist each attr_names,'enlist each attr_tst_data;
// Attempt to write to an already existing attribute
test_fail[.hdf5.writeAttr;(fname;attr_dset;"num attr";num_tst)]
// Attempt to write an attribute with an unsupported type
(all/)test_fail[.hdf5.writeAttr;]each attr_data
// Ensure the attribute wasn't accidentally created
(all/)test_fail[.hdf5.isAttr;]each neg[1]_'attr_data
str_tst ~ .hdf5.readAttr[fname;attr_dset;"str attr"]
num_tst ~ .hdf5.readAttr[fname;attr_dset;"num attr"]
-1"Testing attribute functions complete\n";

-1"Testing linking functionality";
n_loc:"new_path"
.hdf5.createSoft[fname;n_loc;"dset_1_0"]
.hdf5.readData[fname;"dset_1_0"] ~ .hdf5.readData[fname;n_loc]
.hdf5.delLink[fname;n_loc];
.hdf5.createHard[fname;n_loc;"dset_5_10"]
.hdf5.readData[fname;"dset_5_10"] ~ .hdf5.readData[fname;n_loc]
-1"Testing linking functionality complete\n";

-1"Testing utility functions";
// Create a file to check function of ishdf5
`:test_txt.txt 0: enlist "text to save";

not .hdf5.ishdf5["test_txt.txt"]
.hdf5.ishdf5[fname]

.hdf5.getDataShape[fname;"dset_5_6"] ~ 5 4 3 2 1 
-1"Testing utility functions complete\n";

// Clean up
$[.z.o like "w*";system "del test_*";system"rm test_*"];
