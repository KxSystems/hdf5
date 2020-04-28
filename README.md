# `hdf5-kdb` – Interface for the conversion of between the hdf5 data format and kdb+ data 

This is a thin wrapper for kdb+ around the [HDF Groups](https://support.hdfgroup.org/) C api for HDF5.

See [here](https://github.com/cmccarthy1/docs/tree/q_hdf5/docs/interfaces/hdf5) for full documentation.

## 

This interface is supported for the following platforms

* 64bit Linux
* 64bit Mac

## Installation


The following sections outline the instructions for building the interface for various architectures. In each case please ensure that `$QHOME/%QHOME%` is set in order to ensure make/bat files install the required binaries in an appropriate location

### Linux
1. Download a release of hdf5 and install, instructions are provided [here](https://support.hdfgroup.org/HDF5/HDF5-FAQ.html#10) but left as an exercise for a user.
2. Ensure that both the `.h` and `.lib` files are placed on your `LD_LIBRARY_PATH` such that they can be included within the build.
3. From this directory run `make && make install && make clean`.

### MacOs
1. To install hdf5 run `brew install hdf5`
2. From this directory run `make && make install && make clean`

**Note:** There may be a need to run `export HDF5_DISABLE_VERSION_CHECK=2` in cases where there is a clash between the header file and functionality which can manifest at install. If this arises with personal/production builds please open an issue within the repository.

## Examples

Examples of all the functions presently within the interface can be found [here](https://github.com/cmccarthy1/docs/blob/q_hdf5/docs/interfaces/hdf5/user-guide.md). While this the example execution of each of the functions it is maintained in parallel to updates to this code base and as such may go out of sync from time to time.
