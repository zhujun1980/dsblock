enable_testing()
SET(INSTALL_GTEST OFF CACHE BOOL "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest")

file(GLOB UNIT_TEST_SOURCE "*.cc" "*.h")

include_directories("googletest/googletest/include"
                    "googletest/googlemock/include")

add_executable(${PROJECT_NAME}-unit-tests ${UNIT_TEST_SOURCE})

target_link_libraries(${PROJECT_NAME}-unit-tests
                      gtest
                      gtest_main
                      dsblock-static-lib)

add_test(AllTestsIn${PROJECT_NAME}UnitTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-unit-tests)
