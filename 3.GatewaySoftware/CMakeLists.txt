project (Gateway C CXX)
cmake_minimum_required (VERSION 3.0)
include(ExternalProject)

# IMPORTANT ! SET install directory
set (JEVONALARMCLOCK_INSTALL /opt/jevonalarmclock)
set (CMAKE_INSTALL_PREFIX ${JEVONALARMCLOCK_INSTALL})
set (CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" )
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${JEVONALARMCLOCK_INSTALL}/include")
set(ENV{PATH} "${JEVONALARMCLOCK_INSTALL}/bin:$ENV{PATH}")

#build 3rd party libraries
add_subdirectory(external)

#include folder for 3rd party libraries
include_directories(${JEVONALARMCLOCK_INSTALL}/include)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -pthread " )
MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
SET(CMAKE_INSTALL_RPATH "../lib")
MESSAGE(STATUS "CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}")

#include folder for 3rd party libraries
include_directories(include)

################################
# GTest
################################
ADD_SUBDIRECTORY (external/download/gtest-1.7.0)
enable_testing()


# Source Subdirectories
add_subdirectory(src)

## Test
add_subdirectory(test)

INSTALL(DIRECTORY include/ DESTINATION ${JEVONALARMCLOCK_INSTALL}/include )
