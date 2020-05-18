#!/bin/bash

if [ "$TRAVIS_OS_NAME" == "osx" ]; then # use homebrew version
  brew update
  brew install hdf5
  echo "brew install finished"
else # install from source
  wget "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.4/src/hdf5-1.10.4.tar.gz"
  tar -xzf "hdf5-1.10.4.tar.gz"
  cd "hdf5-1.10.4"
  mkdir $TRAVIS_BUILD_DIR/cbuild
  if [ "$TRAVIS_OS_NAME" == "linux" ]; then
    ./configure --prefix=$TRAVIS_BUILD_DIR/cbuild
    sudo make install 2>&1 | tail -n200
  else
    # Windows build
    mkdir cmake && cd cmake
    cmake -G "Visual Studio 15 2017 Win64" ..
    export MSBUILD_PATH="/c/Program Files (x86)/Microsoft Visual Studio/2017/BuildTools/MSBuild/15.0/Bin/";
    export PATH=$MSBUILD_PATH:$PATH;
    MSBuild.exe INSTALL.vcxproj //m //nologo //verbosity:normal //p:Configuration=Release //p:Platform=x64;
    cd ..
  fi
  cd ..
fi
