# Environment variable for HDF5_HOME is required
if(NOT DEFINED ENV{HDF5_HOME})
	message(FATAL_ERROR "HDF5_HOME must be set to the location of your installed hdf5-group repository")
endif()

# Default such that 'make install' moves binary & q script to QHOME
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE MinSizeRel)
ENDIF()

cmake_minimum_required(VERSION 2.8)
project(hdf5-kdb C)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-strict-aliasing -DKXVER=3")

set(MY_LIBRARY_NAME hdf5)
file(GLOB C_FILES src/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

include_directories($ENV{HDF5_HOME}/include)

find_library(HDF5_LIBRARY
    NAMES hdf5 libhdf5
    HINTS "$ENV{HDF5_HOME}/lib/"
)

add_library (${MY_LIBRARY_NAME} SHARED ${C_FILES})
target_link_libraries(${MY_LIBRARY_NAME} ${HDF5_LIBRARY})

# "-undefined dynamic_lookup" needed for building shared libraries on Mac
# MacOS also produces binaries as .dylib, for kdb these should be ".so"
# Setting OSFLAG variable needed for installing binary to $QHOME/<QARCH>
if(APPLE)
   set_target_properties(${MY_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
   set_target_properties(${MY_LIBRARY_NAME} PROPERTIES SUFFIX .so)
   set(INSTALL_SCRIPT install.sh)
   set(OSFLAG m)
elseif(WIN32)
   set(INSTALL_SCRIPT install.bat)
   set(OSFLAG w)
else()
   set(INSTALL_SCRIPT install.sh)
   set(OSFLAG l)
endif()

# Check if 32-bit/64-bit machine
set(BITNESS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BITNESS 64)
endif()

# Minimum build/install move binary and q-script to $QHOME & $QHOME/<QARCH>
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "$ENV{QHOME}/${OSFLAG}${BITNESS}/" CONFIGURATIONS MinSizeRel Release)
install(FILES hdf5.q DESTINATION "$ENV{QHOME}" CONFIGURATIONS MinSizeRel Release)

# Required objects for release packaging
install(FILES LICENSE DESTINATION ${PROJECT_BINARY_DIR}/Release CONFIGURATIONS Release)
install(FILES README.md DESTINATION ${PROJECT_BINARY_DIR}/Release CONFIGURATIONS Release)
install(DIRECTORY examples DESTINATION ${PROJECT_BINARY_DIR}/Release CONFIGURATIONS Release)
install(FILES install.sh DESTINATION ${PROJECT_BINARY_DIR}/Release CONFIGURATIONS Release)
install(FILES hdf5.q DESTINATION ${PROJECT_BINARY_DIR}/Release CONFIGURATIONS Release)
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION ${PROJECT_BINARY_DIR}/Release/lib CONFIGURATIONS Release)
