

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})



################################
# Unit Tests
################################
# Add test cpp file
#add_executable( runUnitTests testgtest.cpp )
# Link test executable against gtest & gtest_main
#target_link_libraries(runUnitTests gtest gtest_main)
#add_test( runUnitTests runUnitTests )

include_directories(${gtest_SOURCE_DIR}/include)
set(test_sources
# files containing the actual tests
TestServer.cpp
)
add_executable(tests ${test_sources})
target_link_libraries(tests gtest_main)
add_test(Tests tests)
