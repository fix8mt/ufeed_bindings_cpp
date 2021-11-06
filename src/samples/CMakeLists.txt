cmake_minimum_required(VERSION 3.14)
set(CMAKE_VERBOSE_MAKEFILE FALSE)
project(ufecpp_samples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Generate debug library name with a postfix.")
option(UFECPP_ENABLE_ADDRESS_SANITIZER    "enable address sanitizer (default=no)" OFF)
set(UFECPP_ROOT "/opt/ufecpp" CACHE STRING "UFECPP install path")

add_executable(sample0 sample0.cpp)
target_include_directories(sample0 PRIVATE ${UFECPP_ROOT}/include)
target_link_directories(sample0 PRIVATE ${UFECPP_ROOT}/lib)
target_link_libraries(sample0 PRIVATE debug libufeedclientd.so debug libprotobuf-lited.so optimized libufeedclient.so optimized libprotobuf-lite.so uuid pthread)
