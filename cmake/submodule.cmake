set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)
include(cmake/gcc.cmake)
include(cmake/msvc.cmake)

# gtest
set(BUILD_GTEST ON CACHE BOOL "enable gtest")
set(BUILD_GMOCK OFF CACHE BOOL "disable gmock")
add_subdirectory(3rdparty/googletest/googletest)
include_directories(cmake/3rdparty/googletest/googletest/include)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "global_filename_not_set.log;global_filename_not_set.log1;global_filename_not_set.log2;_persister_test.log;utest_plog.log;utest_slog.log")

# zmq
set(BUILD_SHARED OFF CACHE BOOL "disable zmq shared build")
set(BUILD_STATIC ON CACHE BOOL "enable zmq static build")
set(BUILD_TESTS OFF CACHE BOOL "disable zmq tests build")
set(WITH_DOCS OFF CACHE BOOL "disable zmq docs build")
add_subdirectory(3rdparty/libzmq EXCLUDE_FROM_ALL)
set(${project_prefix}_ZMQ_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libzmq)

# protobuf
set(protobuf_BUILD_TESTS OFF CACHE BOOL "disbale protobuf test build")
set(protobuf_BUILD_CONFORMANCE OFF CACHE BOOL "disbale protobuf conformance build")
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "disbale protobuf examples build")
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "build libprotoc and protoc compiler")
set(protobuf_BUILD_SHARED_LIBS ON CACHE BOOL "enable protobuf shared libs build")
add_subdirectory(3rdparty/protobuf/cmake EXCLUDE_FROM_ALL)
set(${project_prefix}_PROTOBUF_PATH ${CMAKE_SOURCE_DIR}/3rdparty/protobuf)

# cppzmq
set(${project_prefix}_CPPZMQ_PATH ${CMAKE_SOURCE_DIR}/3rdparty/cppzmq)

# cxxopt
# set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "disable cxx opts examples build")
# set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "disable cxx opts tests build")
# add_subdirectory(3rdparty/cxxopts EXCLUDE_FROM_ALL)
