project (Gateway C CXX)
cmake_minimum_required (VERSION 2.8)
include(ExternalProject)

# IMPORTANT ! SET install directory
set (JEVONALARMCLOCK_INSTALL /opt/jevonalarmclock)
set (CMAKE_INSTALL_PREFIX ${JEVONALARMCLOCK_INSTALL})
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
# Source Subdirectories
add_subdirectory(src)

INSTALL(DIRECTORY include/ DESTINATION ${JEVONALARMCLOCK_INSTALL}/include )
