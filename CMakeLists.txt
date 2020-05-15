# Environment variable for HDF5_HOME is required
if(NOT DEFINED ENV{BUILD_HOME})
	message(FATAL_ERROR "BUILD_HOME must be set to the location of your installed hdf5-group repository")
endif()

# Default such that 'make install' moves binary & q script to QHOME
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE MinSizeRel)
ENDIF()

cmake_minimum_required(VERSION 3.0)
project(hdf5 C)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DKXVER=3")

set(MY_LIBRARY_NAME hdf5)
file(GLOB C_FILES src/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)

message(STATUS "Generator : ${CMAKE_GENERATOR}")
message(STATUS "Build Tool : ${CMAKE_BUILD_TOOL}")
message(STATUS "HDF5 API : $ENV{BUILD_HOME}")

include_directories($ENV{BUILD_HOME}/include)

find_library(HDF5_LIBRARY
    NAMES hdf5 libhdf5
    HINTS "$ENV{BUILD_HOME}/lib/" 
)

if (MSVC)
    file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/w64/q.lib" "${CMAKE_BINARY_DIR}/q.lib" )
    set(LINK_LIBS "${CMAKE_BINARY_DIR}/q.lib")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /D H5_BUILT_AS_DYNAMIC_LIB /D WIN32_LEAN_AND_MEAN /D _CRT_SECURE_NO_WARNINGS")
else()
    set(LINK_LIBS "")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-aliasing")
endif()

add_library (${MY_LIBRARY_NAME} SHARED ${C_FILES})
target_link_libraries(${MY_LIBRARY_NAME} ${HDF5_LIBRARY} ${LINK_LIBS})
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES PREFIX "")

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

message(STATUS "Installing package to : ${PROJECT_BINARY_DIR}/hdf5")

# Minimum build/install move binary and q-script to $QHOME & $QHOME/<QARCH>
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION "$ENV{QHOME}/${OSFLAG}${BITNESS}/" CONFIGURATIONS MinSizeRel Release)
install(DIRECTORY "q/" DESTINATION "$ENV{QHOME}" CONFIGURATIONS MinSizeRel Release FILES_MATCHING PATTERN "*.q")

install(FILES README.md LICENSE ${INSTALL_SCRIPT} DESTINATION ${PROJECT_BINARY_DIR}/hdf5 CONFIGURATIONS Release)
install(DIRECTORY examples DESTINATION ${PROJECT_BINARY_DIR}/hdf5 CONFIGURATIONS Release)
install(DIRECTORY q DESTINATION ${PROJECT_BINARY_DIR}/hdf5 CONFIGURATIONS Release)
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION ${PROJECT_BINARY_DIR}/hdf5/lib CONFIGURATIONS Release)
