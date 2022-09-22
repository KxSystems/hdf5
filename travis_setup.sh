#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "osx" ]; then # use homebrew version
  echo "Updating brew"
  brew update
  echo "Installing hdf5"
  brew install hdf5@1.10
  echo "brew install finished"
  export HDF5_INSTALL_DIR="/usr/local/opt/hdf5@1.10/"
else # install from source
  wget --no-check-certificate "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.9/src/hdf5-1.10.9.tar.gz"
  tar -xzf "hdf5-1.10.9.tar.gz"
  cd "hdf5-1.10.9"
  export HDF5_INSTALL_DIR=$TRAVIS_BUILD_DIR/cbuild
  mkdir $HDF5_INSTALL_DIR
  if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    ./configure --prefix=$HDF5_INSTALL_DIR
    sudo make install 2>&1 | tail -n200
  else
    # Windows build
    mkdir cmake && cd cmake
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=$HDF5_INSTALL_DIR .. -DBUILD_TESTING:BOOL=OFF
    cmake --build . --config Release
    cmake --build . --config Release --target install
    cd ..
  fi
  cd ..
fi
