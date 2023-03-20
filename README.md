![hdf5](docs/img/hdf5.jpeg)

# Interface for conversion between HDF5 and kdb+ data 

[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kxsystems/hdf5?include_prereleases)](https://github.com/kxsystems/hdf5/releases) [![Travis (.org) branch](https://img.shields.io/travis/kxsystems/hdf5/master?label=travis%20build)](https://travis-ci.org/kxsystems/hdf5/branches)

## Introduction

This interface lets kdb+ users convert data between HDF5 and kdb+ in order to allow users familiar with either technology to analyze data in a format which they may be more familiar with.

The interface follows closely the [HDF5 C API](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5Front.html). In certain cases structures or types common in HDF5 or q may by necessity need to be modified to account for differences between the two data formats. In cases where this is necessary, documentation will indicate the assumptions made and expected behavior. This is particularly the case in the [mapping of types](docs/hdf5-types.md).

Exposed functionality includes

-   creation of files/attributes/datasets/groups/links
-   read/write HDF5 data or attributes from a kdb+ session
-   link locations within files

This is part of the [_Fusion for kdb+_](http://code.kx.com/q/interfaces/#fusion-interfaces/) interface collection.

### Use cases

This data format is used across a number of sectors with use-cases including but limited to the storage of

-   Neural network architectures (Python Keras)
-   Geological and Hydrological datasets.
-   Astrophysical earth observation data for NASA

Further information on sectors which make use of this technology can be found [here](https://support.hdfgroup.org/HDF5/users5.html)

## New to kdb+ ?

Kdb+ is the world's fastest time-series database, optimized for ingesting, analyzing and storing massive amounts of structured data. To get started with kdb+, please visit https://code.kx.com/q for downloads and developer information. For general information, visit https://kx.com/

## New to HDF5 ?

[Hierarchical Data Format 5 (HDF5)](https://portal.hdfgroup.org/display/HDF5/HDF5) is a file format designed specifically for the storage and organization of large amounts of data. It is supported by the HDF Group and free to use under a BSD-like licence. 

In many ways HDF5 acts like a truly hierarchical file system similar to that used by Linux or Windows. This structure contains two primary objects:

**Datasets** may contain multidimensional arrays of homogenously-typed data or compound data containing a mixture of types. These act similarly to files within a traditional POSIX file system.

**Groups** act as container structures which can hold datasets or other further groups. These act similarly to folders in a traditional POSIX file system.

A number of secondary objects and structures add complexity to the format and in doing so allow the format to be used for a wider number of use cases.

**Attributes** allow metadata information to be associated with a dataset or group, i.e. associate the date of data collection with a group or the room temperature at the time a set of results was collected.

**Linking functionality** Like a traditional POSIX file system, it is possible to create links between objects (hard/soft/external); these allow datasets or groups relevant to multiple experiments or use cases to be accessed via routes that a user may find more intuitive.


If you have any HDF5 related questions, you can raise them on the [HDF Forum](https://forum.hdfgroup.org/).

## API Documentation

:point_right: [`API reference`](docs/reference.md)

## Mapping of HDF5 Types

:point_right: [`Type Mapping`](docs/hdf5-types.md)

## Installation Documentation

:point_right: [`Install guide`](docs/install.md)

## Building From Source

:point_right: [`Building guide`](docs/build.md)

## Example Usage

:point_right: [`Examples`](docs/example.md)

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

## Status

This interface is currently available under an Apache 2.0 licence and is supported on a best-efforts basis by the Fusion team. This interface is currently in active development, with additional functionality to be released on a rolling basis.

If you find issues with the interface or have feature requests please [raise an issue](../../issues).

To contribute to this project please follow the [contribution guide ](CONTRIBUTING.md).
