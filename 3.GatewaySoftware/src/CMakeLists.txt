
add_executable(Application Main.cpp time/Time.cpp serialport/SerialPort.cpp serialport/IoServiceProvider.cpp http/HueClient.cpp)
target_link_libraries(Application ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Application ${BOOST_LIBRARIES}) 
target_link_libraries(Application ${POCO_LIBRARIES})
target_link_libraries(Application ${CURL_LIBRARIES})

INSTALL(TARGETS Application
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)


