cmake_minimum_required(VERSION 3.0.0)
enable_testing()
set(CMAKE_MACOSX_RPATH 1)

set(DSBLOCK_VERSION "0.1.0")
message("-- Build libdsblock (${DSBLOCK_VERSION})")
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" DSBLOCK_BASE_VERSION "${DSBLOCK_VERSION}")
project(dsblock VERSION "${DSBLOCK_BASE_VERSION}"
        LANGUAGES C CXX)

set(DSBLOCK_VERSION_MAJOR ${dsblock_VERSION_MAJOR})
set(DSBLOCK_VERSION_MINOR ${dsblock_VERSION_MINOR})
set(DSBLOCK_VERSION_PATCH ${dsblock_VERSION_PATCH})

include_directories(SYSTEM
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/src)

if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -fPIC -static-libgcc -static-libstdc++ -lpthread -lm -ldl")
    SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g  -Wall -fPIC")
endif()

include(TestBigEndian)
test_big_endian(BIG_ENDIAN)
if(BIG_ENDIAN)
    add_definitions(-DCMAKE_LITTLE_ENDIAN=0)
else()
    add_definitions(-DCMAKE_LITTLE_ENDIAN=1)
endif()

SET(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
SET(CMAKE_BUILD_RPATH   "/usr/lib:/usr/local/lib:$ORIGIN/")
SET(CMAKE_INSTALL_RPATH "/usr/lib:/usr/local/lib:$ORIGIN/")

set(DSBLOCK_SRC
        src/c_api.cc
        src/log.cc
        src/tst.cc
        src/utf8.cc
        src/bit_set.cc
        src/bloom_filter.cc
        src/file_utils.cc
        src/murmur_hash3.cc
        src/rand_utils.cc
        src/shmm_posix.cc
        src/time_utils.cc
)

add_library(dsblock-lib SHARED
        ${DSBLOCK_SRC})

add_library(dsblock-static-lib STATIC
        ${DSBLOCK_SRC})

set_target_properties(dsblock-lib PROPERTIES
        OUTPUT_NAME "dsblock"
        PREFIX      "lib")

set_target_properties(dsblock-static-lib PROPERTIES
        OUTPUT_NAME "dsblock"
        PREFIX      "lib")

configure_file("src/config.h.cmake" "config.h")

install(TARGETS dsblock-lib DESTINATION "lib")
install(TARGETS dsblock-static-lib DESTINATION "lib")
install(FILES "src/c_api.h"
              "${PROJECT_BINARY_DIR}/config.h"
        DESTINATION "include/dsblock")

add_subdirectory(test)
add_subdirectory(python)
