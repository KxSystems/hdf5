![hdf5](docs/img/hdf5.jpeg)

# Interface for conversion between HDF5 and kdb+ data 

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kxsystems/hdf5?include_prereleases)](https://github.com/kxsystems/hdf5/releases) [![Travis (.org) branch](https://img.shields.io/travis/kxsystems/hdf5/master?label=travis%20build)](https://travis-ci.org/kxsystems/hdf5/branches)

## Introduction

This interface provides a mechanism for kdb+ users to interact with and create HDF5 datasets. The interface is a thin wrapper for kdb+ around the HDF Group’s C API for HDF5, outlined in full [here](https://portal.hdfgroup.org/pages/viewpage.action?pageId=50073943)

This is part of the [_Fusion for kdb+_](http://code.kx.com/q/interfaces/fusion/) interface collection.

## New to kdb+ ?

Kdb+ is the world's fastest time-series database, optimized for ingesting, analyzing and storing massive amounts of structured data. To get started with kdb+, please visit https://code.kx.com/q for downloads and developer information. For general information, visit https://kx.com/

## New to HDF5 ?

Hierarchical Data Format 5 (HDF5) is a file format designed specifically for the storage and organization of large amounts of data.

In many ways, HDF5 acts like a hierarchical file system similar to that used by Linux or Windows. This structure contains two major objects:

1. Datasets - multidimensional arrays of homogenously-typed data, or compound data containing a mixture of types. Datasets are similar to files within a traditional POSIX file system.
2. Groups - container structures holding datasets or other groups. They function similarly to directories within a traditional POSIX file system.

There are a number of secondary objects and structures, which add complexity to the format. In doing so, they allow the format to be used for a wider range of use cases.

* Attributes: allow metadata information to be associated with a dataset or group e.g. associate the date of data collection with a group, or the temperature when a set of results was collected.
* Linking functionality: as with a traditional POSIX file system, it is possible to create links between objects (hard/soft/external). These allow datasets or groups relevant to multiple experiments to be accessed via more intuitive routes.

If you have any HDF5 related questions, you can raise them on the [HDF Forum](https://forum.hdfgroup.org/).


## Installation

### Requirements

* kdb+ ≥ 3.5 64-bit
* [HDF5 C API](https://portal.hdfgroup.org/display/support/Downloads) ≥ 1.10.9

### Installing a release

We recommend you install this interface through a release:

1. Ensure you have downloaded/installed the HDF groups C API for HDF5 following the [instructions](#third-party-library-installation).
2. Download a release from [here](../../releases)
3. Install by executing the following from the Release directory. NOTE: by default, the q executable script `q/hdf5.q` and binary file `lib/hdf5kdb.(so|dll)` will be copied to `$QHOME` and `$QHOME/[mlw](64)` but you can override this by setting the environment variables `Q_SCRIPT_DIR` and/or `Q_SHARED_LIB_DIR`.

## Linux/macOS
```bash
chmod +x install.sh && ./install.sh
```

## Windows
```shell
>install.bat
```


### Building the interface from source and install

To build and install this interface, set the following environment variables:

1. `HDF5_INSTALL_DIR` is the location of the HDF5 C API installation (directory containing `/include` and `/lib` subdirectories).
2. `QHOME` is the q installation directory (containing `q.k`).

#### Third-party library installation

Install HDF5 C API according to your architecture.

**Linux**

Download a supported release of HDF5 and install, instructions are provided in the README of the HDF5 packages. 
Then set `HDF5_INSTALL_DIR` to your install directory and add the path to `LD_LIBRARY_PATH`.


```bash
export LD_LIBRARY_PATH=${HDF5_INSTALL_DIR}/lib:$LD_LIBRARY_PATH 
```


**macOS**

```bash
brew install hdf5
```

Then set `HDF5_INSTALL_DIR` to your install directory and add the path to `DYLD_LIBRARY_PATH`.

```bash
export DYLD_LIBRARY_PATH=${HDF5_INSTALL_DIR}/lib:$DYLD_LIBRARY_PATH
```


**Windows**

Install the Windows C API for HDF5, following the [instructions](https://support.hdfgroup.org/HDF5/faq/windows.html). 
Then set `HDF5_INSTALL_DIR` to your install directory and create links to the DLLs in the `%QHOME%\w64` directory. e.g.


```bat
:: Download zip for Windows and unzip it. ex.) hdf5-1.12.0.zip from https://confluence.hdfgroup.org/display/support/HDF5%201.12.0
> cd hdf5[some version]
> mkdir build
> mkdir install
> set HDF5_INSTALL_DIR=%cd%\install
> cd build
build> cmake --config Release -DCMAKE_INSTALL_PREFIX=%HDF5_INSTALL_DIR% .. -DBUILD_TESTING:BOOL=OFF
build> cmake --build . --config Release --target install
build> cd %QHOME%\w64
w64> MKLINK libhdf5.dll %HDF5_INSTALL_DIR%\bin\libhdf5.dll
w64> MKLINK hdf5.dll %HDF5_INSTALL_DIR%\bin\hdf5.dll
```


### Install HDF5 shared object

#### Linux/macOS


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


## :warning: Unsupported functionality

This interface is in active development; as such, there are a number of use cases currently unsupported.

- Creation of compressed datasets
- Access to unlimited datasets
- Interaction with HDF5 images

If your use case requires the above functionality, please [open an issue](../..//issues). If you are able, please consider contributing to the project.

## Threaded/concurrent access

:point_right:  [Thread safety](https://portal.hdfgroup.org/display/knowledge/Questions+about+thread-safety+and+concurrent+access#Questionsaboutthread-safetyandconcurrentaccess-thread-safety) explains when you can use threads (e.g. `peach`), or concurrently process files from multiple instances.

By default, our releases are built without multithread support. 
For multithread support, download the source from our release page to build against your HDF5 library with threading enabled. 
We recommend you use the standard non-thread safe version of HDF5 if not using multiple threads.

## Documentation

:open_file_folder: [`docs`](docs/README.md)

## Status

The HDF5 interface is provided here under an Apache 2.0 license.

If you find issues with the interface or have feature requests please [raise an issue](../../issues).

To contribute to this project please follow the [contribution guide ](CONTRIBUTING.md).
