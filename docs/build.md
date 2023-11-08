# Building HDF5 kdb+ library from source

To build and install this interface, set the following environment variables:

1. `HDF5_INSTALL_DIR` is the location of the HDF5 C API installation (directory containing `/include` and `/lib` subdirectories).
2. `QHOME` is the q installation directory (containing `q.k`).

## Third-party library installation

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

# Install HDF5 shared object

## Linux/macOS

```bash
]$ mkdir build && cd build
build]$ cmake ..
build]$ cmake --build . --target install
```

## Windows

From a Visual Studio command prompt:


```bat
> mkdir build && cd build
build> cmake --config Release ..
build> cmake --build . --config Release --target install
```
