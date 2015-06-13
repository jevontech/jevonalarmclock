#3rd party libraries

find_package(Threads)
MESSAGE("Threads: ${CMAKE_THREAD_LIBS_INIT}")
find_package(Boost 1.46 COMPONENTS system thread program_options serialization REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set(BOOST_LIBRARIES ${Boost_LIBRARIES} PARENT_SCOPE )
  MESSAGE("BOOST libraries: ${Boost_LIBRARIES}")
endif()

find_package(CURL)
if(CURL_FOUND)
 include_directories(${CURL_INCLUDE_DIRS})
 set(CURL_LIBRARIES ${CURL_LIBRARIES} PARENT_SCOPE )
endif()


################
#Poco libraries#
################
ExternalProject_Add(poco
 URL ${CMAKE_CURRENT_SOURCE_DIR}/download/poco-1.6.0-all.tar.gz
  PREFIX ${JEVONALARMCLOCK_INSTALL}
  CONFIGURE_COMMAND <SOURCE_DIR>/configure
     --omit=Data/ODBC,Data/MySQL,Crypto,NetSSL_OpenSSL --no-tests --no-samples --prefix=<INSTALL_DIR>
)
#ExternalProject_Get_Property(poco install_dir)
set(POCO_LIBRARIES ${JEVONALARMCLOCK_INSTALL}/lib/libPocoFoundationd.so ${JEVONALARMCLOCK_INSTALL}/lib/libPocoNetd.so ${JEVONALARMCLOCK_INSTALL}/lib/libPocoZipd.so ${JEVONALARMCLOCK_INSTALL}/lib/libPocoUtild.so ${JEVONALARMCLOCK_INSTALL}/lib/libPocoXMLd.so PARENT_SCOPE )
MESSAGE("Poco libraries: ${POCO_LIBRARIES}")


#Google Protocol buffers
ExternalProject_Add(protobuf
  URL ${CMAKE_CURRENT_SOURCE_DIR}/download/protobuf-cpp.tar.gz
  PREFIX ${JEVONALARMCLOCK_INSTALL}
 # UPDATE_COMMAND <SOURCE_DIR>/autogen.sh
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
#PREFIX ${JEVONALARMCLOCK_INSTALL}
 #CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
)
ExternalProject_Get_Property(protobuf install_dir)
set(PROTOBUF_LIBRARY ${install_dir}/lib/libprotobuf.so ${install_dir}/lib/libprotobuf-lite.so PARENT_SCOPE )
MESSAGE("Protobuffer: protoc ${PROTOBUF_PROTOC_EXECUTABLE}")
