# Minimum CMake version
cmake_minimum_required(VERSION 3.1)

# Package name
project(hdf5 C)

# Shared object name
set(MY_LIBRARY_NAME hdf5kdb)

# Environment variable for HDF5_HOME is required
if(NOT DEFINED ENV{HDF5_INSTALL_DIR})
	message(FATAL_ERROR "HDF5_INSTALL_DIR must be set to the location of your installed hdf5-group repository")
endif()

# Set default build Release
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()

# Check if necessary configuration is set
message(STATUS "Generator : ${CMAKE_GENERATOR}")
message(STATUS "Build Tool : ${CMAKE_BUILD_TOOL}")
message(STATUS "HDF5 API : $ENV{HDF5_INSTALL_DIR}")

# Set target library
file(GLOB C_FILES src/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE C)
add_library (${MY_LIBRARY_NAME} SHARED ${C_FILES})

# Add include directories of this interface and HDF5
target_include_directories(${MY_LIBRARY_NAME} PRIVATE
   ${PROJECT_SOURCE_DIR}/include
   $ENV{HDF5_INSTALL_DIR}/include
)

# Download k.h into include/
file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/c/c/k.h" "${PROJECT_SOURCE_DIR}/include/k.h")

# Search HDF5 library
find_library(HDF5_LIBRARY
    REQUIRED
    NAMES hdf5 libhdf5
    HINTS "$ENV{HDF5_INSTALL_DIR}/lib/" 
)

# Set Base CMake C compile flag
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DKXVER=3")

# Link libraries and additional compie flag
if (MSVC)
    file(DOWNLOAD "https://github.com/KxSystems/kdb/raw/master/w64/q.lib" "${CMAKE_BINARY_DIR}/q.lib" )
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        "${CMAKE_BINARY_DIR}/q.lib"
        ${HDF5_LIBRARY} 
    )
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /D H5_BUILT_AS_DYNAMIC_LIB /D WIN32_LEAN_AND_MEAN /D _CRT_SECURE_NO_WARNINGS")
else()
    target_link_libraries(${MY_LIBRARY_NAME} PRIVATE
        ${HDF5_LIBRARY}
    )
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-strict-aliasing")
endif()

# target_link_libraries(${MY_LIBRARY_NAME} ${HDF5_LIBRARY} ${LINK_LIBS})

# Shared library prefix and suffix
# ex.) telegrafkdb.so for linux
# Suffix must be `.so` for Mac
if(APPLE)
  set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif()
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
set_target_properties(${MY_LIBRARY_NAME} PROPERTIES PREFIX "")

# "-undefined dynamic_lookup" needed for building shared libraries on Mac
# Setting OSFLAG variable needed for installing binary to $QHOME/<QARCH>
if(APPLE)
   set_target_properties(${MY_LIBRARY_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
   set(OSFLAG m)
elseif(WIN32)
   set(OSFLAG w)
else()
   set(OSFLAG l)
endif()

# Check if 32-bit/64-bit machine
set(BITNESS 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BITNESS 64)
endif()

# Always build package
file(COPY README.md LICENSE ${INSTALL_SCRIPT} DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/)
file(COPY examples/ DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/examples/)
file(COPY q/ DESTINATION ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/q/)
# Copy built shared object after build instead of during installation
add_custom_command(TARGET ${MY_LIBRARY_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${MY_LIBRARY_NAME}>" ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}/lib/${MY_LIBRARY_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
    DEPENDS ${MY_LIBRARY_NAME}
)

# Install binary and q-script to $QHOME & $QHOME/<QARCH> at Release
install(TARGETS ${MY_LIBRARY_NAME} DESTINATION $ENV{QHOME}/${OSFLAG}${BITNESS}/ CONFIGURATIONS Release)
install(DIRECTORY q/ DESTINATION $ENV{QHOME} CONFIGURATIONS Release)