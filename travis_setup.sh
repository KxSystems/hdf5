#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "osx" ]; then # use homebrew version
  brew update
  brew install hdf5
  echo "brew install finished"
else # install from source
  cd ..
  wget "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.4/src/hdf5-1.10.4.tar.gz"
  tar -xzf "hdf5-1.10.4.tar.gz"
  cd "hdf5-1.10.4"
  ./configure --prefix=/usr/local
  sudo make install 2>&1 | tail -n200
  cd ..
fi
