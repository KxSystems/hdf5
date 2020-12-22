# Interface for conversion between hdf5 and kdb+ data 

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kxsystems/hdf5?include_prereleases)](https://github.com/kxsystems/hdf5/releases) [![Travis (.org) branch](https://img.shields.io/travis/kxsystems/hdf5/master?label=travis%20build)](https://travis-ci.org/kxsystems/hdf5/branches)

## Introduction

This interface provides a mechanism for kdb+ users to interact with and create HDF5 datasets. The interface is a thin wrapper for kdb+ around the HDF Group's C api for HDF5, outlined in full [here](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5Front.html)

This is part of the [_Fusion for kdb+_](http://code.kx.com/q/interfaces/fusion/) interface collection.

## New to kdb+ ?

Kdb+ is the world's fastest time-series database, optimized for ingesting, analyzing and storing massive amounts of structured data. To get started with kdb+, please visit https://code.kx.com/q/learn/ for downloads and developer information. For general information, visit https://kx.com/

## New to HDF5 ?

Hierarchical Data Format 5 (HDF5) is a file format designed specifically for the storage and organization of large amounts of data.

In many ways, HDF5 acts like a hierarchical file system similar to that used by linux or windows. This structure contains two major objects:

1. Datasets - Multidimensional arrays of homogeneously-typed data, or compound data containing a mixture of types. Datasets are similar to files within a traditional POSIX file system.
2. Groups - Container structures holding datasets or other groups. They function similarly to directories within a traditional POSIX file system.

There are a number of secondary objects and structures, which add complexity to the format. In doing so, they allow the format to be used for a wider range of use cases

* Attributes: These allow metadata information to be associated with a dataset or group e.g. associate the date of data collection with a group, or the temperature when a set of results was collected.
* Linking functionality: Like a traditional POSIX file system it is possible to create links between objects (hard/soft/external). These allow datasets or groups relevant to multiple experiments to be accessed via more intuitive routes.

If you have any HDF5 related questions, you can raise them on the [HDF Forum](https://forum.hdfgroup.org/).


## Installation

### Requirements

* kdb+ ≥ 3.5 64-bit
* [HDF5 C api](https://support.hdfgroup.org/HDF5/doc/H5.intro.html) ≥ 1.10.4

### Installing a release

It is recommended that a user install this interface through a release. This is completed in a number of steps

1. Ensure you have downloaded/installed the HDF groups C api for HDF5 following the instructions [here](https://github.com/KxSystems/hdf5#third-party-library-installation)
2. Download a release from [here](https://github.com/KxSystems/hdf5/releases)
3. Install required q executable script `q/hdf5.q` and binary file `lib/hdf5kdb.(so|dll)` to `$QHOME` and `$QHOME/[mlw](64)`, by executing the following from the Release directory

```bash

## Linux/MacOS
$ chmod +x install.sh && ./install.sh

## Windows
>install.bat

```

### Building interface from source and install

In order to successfully build and install this interface, the following environment variables must be set:

1. `HDF5_INSTALL_DIR` = Location of the HDF5 C api installation (directory containing `/include` and `/lib` subdirectories).
2. `QHOME` = Q installation directory (directory containing `q.k`).

#### Third-Party Library Installation

Install HDF5 C API according to your architecture.

**Linux**

Download a supported release of hdf5 and install, instructions are provided [here](https://support.hdfgroup.org/HDF5/HDF5-FAQ.html#10). Then set `HDF5_INSTALL_DIR` to your install directory and add the path to `LD_LIBRARY_PATH`.

```bash

$ export LD_LIBRARY_PATH=${HDF5_INSTALL_DIR}/lib:$LD_LIBRARY_PATH 

```

**MacOS**

Run:

```bash

$ brew install hdf5

```

Then set `HDF5_INSTALL_DIR` to your install directory and add the path to `DYLD_LIBRARY_PATH`.

```bash

$ export DYLD_LIBRARY_PATH=${HDF5_INSTALL_DIR}/lib:$DYLD_LIBRARY_PATH

```

**Windows**

Install the windows C api for HDF5 following the instructions provided [here](https://support.hdfgroup.org/HDF5/faq/windows.html). Then set `HDF5_INSTALL_DIR` to your install directory and create links to the dll's in the %QHOME%\w64 directory. e.g.

```bat

> cd %QHOME%\w64
> MKLINK libhdf5.dll %HDF5_INSTALL_DIR%\lib\libhdf5.dll
> MKLINK hdf5.dll %HDF5_INSTALL_DIR%\lib\hdf5.dll

```

### Instaling hdf5 shared object

#### Linux/MacOS

```bash

]$ mkdir build && cd build
build]$ cmake ..
build]$ cmake --build . --target install

```

#### Windows

From a Visual Studio command prompt:

```bat

> mkdir build && cd build
build> cmake --config Release ..
build> cmake --build . --config Release --target install

```

## Unsupported Functionality

This interface is in active developement and as such there are a number of use-cases that are currently not supported.

- Creation of compressed datasets
- Access to unlimited datasets
- Interaction with HDF5 images

If your use case requires the above functionality to be available, please open an issue [here](https://github.com/KxSystems/hdf5/issues). If you are capable, please consider contributing to the project.

## Documentation

Documentation outlining the functionality available for this interface can be found [here](https://code.kx.com/q/interfaces/hdf5).

## Status

The HDF5 interface is provided here under an Apache 2.0 license.

If you find issues with the interface or have feature requests please consider raising an issue [here](https://github.com/KxSystems/hdf5/issues).

If you wish to contribute to this project please follow the contributing guide [here](https://github.com/KxSystems/hdf5/blob/master/CONTRIBUTING.md).
