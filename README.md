# `hdf5-kdb` – Interface for the conversion of between the hdf5 data format and kdb+ data 

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kxsystems/hdf5-kdb)](https://github.com/kxsystems/hdf5-kdb/releases) [![Travis (.org) branch](https://img.shields.io/travis/kxsystems/hdf5-kdb/master?label=travis%20build)](https://travis-ci.org/kxsystems/hdf5-kdb/branches)

This is a thin wrapper for kdb+ around the [HDF Groups](https://support.hdfgroup.org/) C api for HDF5.

See [here](https://github.com/cmccarthy1/docs/tree/q_hdf5/docs/interfaces/hdf5) for full documentation.

## 

This interface is supported for the following platforms

* 64bit Linux
* 64bit Mac

## Installation


The following sections outline the instructions for building the interface for various architectures. In each case please ensure that `$QHOME` is set in order to ensure cmake file installs the required binary in an appropriate location.

### Installing HDF5

**Linux**
- Download a release of hdf5 and install, instructions are provided [here](https://support.hdfgroup.org/HDF5/HDF5-FAQ.html#10).

**MacOS**
- To install hdf5 run `brew install hdf5`

### Creating binaries and install
1. Ensure you have set a environment variable `HDF5_HOME` which is the location of the `include/` and `lib/` directories needed for the `HDF5` C api.
2. From the root directory of this library create a directory named `build` and move into this directory `cd build`
3. From the `build` directory run `cmake .. && make install && make clean`.

## Examples

Examples of all the functions presently within the interface can be found [here](https://github.com/cmccarthy1/docs/blob/q_hdf5/docs/interfaces/hdf5/user-guide.md). While this the example execution of each of the functions it is maintained in parallel to updates to this code base and as such may go out of sync from time to time.
