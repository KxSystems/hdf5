# Interface for conversion between hdf5 and kdb+ data 

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kxsystems/hdf5)](https://github.com/kxsystems/hdf5/releases) [![Travis (.org) branch](https://img.shields.io/travis/kxsystems/hdf5/master?label=travis%20build)](https://travis-ci.org/kxsystems/hdf5/branches)

## Introduction

This interface provides a mechanism for kdb+ users to interact with and create HDF5 datasets. The interface is a thin wrapper for kdb+ around the HDF Group's C api for HDF5, outlined in full [here](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5Front.html)

This is part of the [_Fusion for kdb+_](http://code.kx.com/q/interfaces/fusion/) interface collection.

## New to kdb+ ?

Kdb+ is the world's fastest time-series database, optimized for ingesting, analyzing and storing massive amounts of structured data. To get started with kdb+, please visit https://code.kx.com/q/learn/ for downloads and developer information. For general information, visit https://kx.com/

## New to HDF5 ?

Hierarchical Data Format 5 (HDF5) is a file format designed specifically for the storage and organization of large amounts of data.

In many ways, HDF5 acts like a hierarchical file system similar to that used by linux or windows. This structure contains two major objects:

1. Datasets - These may contain multidimensional arrays of homogeneously-typed data, or compound data containing a mixture of types. This is similar to files within a traditional POSIX file system.
2. Groups - These act as container structures, which can hold datasets or other groups. They act similarly to directories in a traditional POSIX file system.

There are a number of secondary objects and structures, which add complexity to the format. In doing so, they allow the format to be used for a wider range of use cases

* Attributes: These allow metadata information to be associated with a dataset or group e.g. Associate the date of data collection with a group, or the room temperature when a set of results was collected.
* Linking functionality: Like a traditional POSIX file system it is possible to create links between objects (hard/soft/external). These allow datasets or groups relevant to multiple experiments or use-cases to be accessed via routes that a user may find more intuitive.

If you have any HDF5 related questions, you can raise them on the [HDF Forum](https://forum.hdfgroup.org/).


## Installation

### Requirements

* kdb+ ≥ 3.5 64-bit
* [HDF5 C api](https://support.hdfgroup.org/HDF5/doc/H5.intro.html) ≥ 1.10

### Third-Party Library Installation

The following outlines instructions for installing the HDF5 group's C api for supported architectures.

**Linux**
- Download a supported release of hdf5 and install, instructions are provided [here](https://support.hdfgroup.org/HDF5/HDF5-FAQ.html#10).

**MacOS**
- To install hdf5 run `brew install hdf5`


### Interface build and install

In order to successfully build and install this interface the following environment variables must be set:

1. `HDF5_HOME` = Location of the HDF5 C apis' `/include` and `/lib` directories.
2. `QHOME` = Location of the q executable 

* Create a directory from which the execute the CMAKE command and move into this directory

```bash
mkdir build && cd build
```

* Execute the `cmake` instructions

```bash
cmake ..
```

* generate the `libhdf5.so` binary

```bash
make
```

* Install the `libhdf5.so` binary into `$QHOME/[ml]64` and `hdf5.q` into `$QHOME`

```bash
make install
```
 
## Unsupported Functionality

This interface is in active developement and as such there are a number of use-cases that are currently not supported.

- Use of this interface on Windows 64-bit systems
- Creation of compressed datasets
- Access to unlimited datasets
- Interaction with HDF5 images

If your use case requires the above functionality to be available, please open an issue [here](https://github.com/KxSystems/hdf5/issues). If you are capable, please consider contributing to the project.

## Documentation

Documentation outlining the functionality available for this interface can be found [here](https://code.kx.com/q/interfaces/hdf5).

## Status

The HDF5 interface is still in development and is provided here as a beta release under an Apache 2.0 license.

If you find issues with the interface or have feature requests please consider raising an issue [here](https://github.com/KxSystems/hdf5/issues).

If you wish to contribute to this project please follow the contributing guide [here](https://github.com/KxSystems/hdf5/blob/master/CONTRIBUTING.md).
