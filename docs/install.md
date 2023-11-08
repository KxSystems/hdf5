# HFD5 kdb+ library installation

## Requirements

* kdb+ ≥ 3.5 64-bit
* [HDF5 C API](https://portal.hdfgroup.org/display/support/Downloads) ≥ 1.10.9

## Installing a release

We recommend you install this interface through a release:

1. Ensure you have downloaded/installed the HDF groups C API for HDF5 following the [instructions](#third-party-library-installation).
2. Download a release from [here](../../releases)
3. Install by executing the following from the Release directory. NOTE: by default, the q executable script `q/hdf5.q` and binary file `lib/hdf5kdb.(so|dll)` will be copied to `$QHOME` and `$QHOME/[mlw](64)` but you can override this by setting the environment variables `Q_SCRIPT_DIR` and/or `Q_SHARED_LIB_DIR`.

### Linux/macOS
```bash
chmod +x install.sh && ./install.sh
```

### Windows
```shell
>install.bat
```
