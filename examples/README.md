# Examples

The scripts outlined here provide examples of the expected usage of this interface. Each script deals with a different aspect of the interface and showcases a particular section of the hdf5 library.

A more basic example workflow is highlighted in the documentation for this interface [here](https://code.kx.com/q/interfaces/hdf5/examples).

## Running examples

To run the entire set of examples please run the following:

```bash
./all.sh
```

In order to execute the shell script as above a user may be required to make the script executable

```bash
chmod +x all.sh
```

The following section provides a breakdown of the functionality exposed by each of the example scripts, showing the command to be executed from command line to run the individual scripts.

## Example Scripts

### `h5_file.q`

File creation example

* Create a HDF5 file named `'test.h5'`
* Highlight to a user that this file exists
* Remove the `'test.h5'` file

```bash
q h5_file.q
```

### `h5_dset.q`

Writing to and reading from a dataset

* Create a HDF5 file named `'test.h5'` 
* Write a dataset of dimensions `3 x 100000` to a dataset named `'dset'`
* Read the dataset 'dset' to kdb+
* Show equivalence between the written and read datasets
* Remove the `'test.h5'` file

```bash
q h5_dset.q
```

### `h5_group.q`

Create groups and display file structure

* Create a HDF5 file named `'test.h5'` 
* Display structure of the file using `.hdf5.ls`
* Create a set of groups 'Group1/SG1' and 'Group1/SG1/SG2'
* Display structure of the file after group creation using `.hdf5.ls`
* Remove the `'test.h5'` file

```bash
q h5_group.q
```

### `h5_attr.q`

Write data to an attribute

* Create a HDF5 file named `'test.h5'`
* Write data to a dataset 'dset'
* Write data to two attributes 'Units' and 'Temperatures at sensors'
* Check for the existence of the attributes using `.hdf5.isAttr`
* Read the attributes back to kdb+
* Remove the `'test.h5'` file

```bash
q h5_attr.q
```

### `h5_comp.q`

Display the reading of data from different standard compression mechanisms

* Read data compressed using szip
* Read data compressed using gzip
* Read data compressed using Scale-offset compression
* Read data compressed using N-Bit compression

```bash
q h5_comp.q
```

### `h5_link.q`

Highlight use of soft linking and dataset copying

* Create a HDF5 file named `'test.h5'`
* Write data to a dataset 'initial_dset'
* Retrieve the size of the file containing 'initial_dset'
* Create a soft link between 'inital_dset' and 'soft_link_set'
* Check that the size of the file has not increased and the data read via each path is equivalent
* Use `.hdf5.copyObject` to make a copy of 'initial_dset' to 'copied_dset'
* Show the increase in file size with the copied object and show 'initial_dset' is equivalent to 'copied_dset'
* Remove the `'test.h5'` file

```bash
q h5_link.q
```

### `h5_tables.q`

Highlight the reading of simple compound datasets from HDF5 and the interface method of reading/writing kdb+ tables to HDF5

* Read a truly compound dataset from HDF5 into kdb+
* Write this compound dataset back to HDF5 from kdb+
* Display the file structure showing differences between the storage mechanisms

```bash
q h5_tables.q
```

### `h5_utils.q`

Highlight utility functions provided with the interface

* What version of the HDF5 C api is being used
* Run HDF5 garbage collection on the process
* Create a dataset and get its size in MB
* Remove the `'test.h5'` file

```bash
q h5_utils.q
```

