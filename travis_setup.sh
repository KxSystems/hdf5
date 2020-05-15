#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "osx" ]; then # use homebrew version
  brew update
  brew install hdf5
  echo "brew install finished"
else # install from source
  wget "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.4/src/hdf5-1.10.4.tar.gz"
  tar -xzf "hdf5-1.10.4.tar.gz"
  cd "hdf5-1.10.4"
  if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    ./configure --prefix=/usr/local
    sudo make install 2>&1 | tail -n200
  else
    # Windows build
    mkdir cmake && cd cmake
    export BUILD_HOME=$TRAVIS_BUILD_DIR/cbuild
    mkdir $BUILD_HOME
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=$BUILD_HOME ..
    cmake --build . --config Release
    cmake --build . --config Release --target install
    cd ..
  fi
  cd ..
fi
