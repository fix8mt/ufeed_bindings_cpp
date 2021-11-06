cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE FALSE)
project(ufecppbinding)
set(project_prefix UFECPP)

include(cmake/submodule.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS NO)
if(${CMAKE_CXX_COMPILER_VERSION} MATCHES "^3.+" OR "${CMAKE_CXX_COMPILER_VERSION}" MATCHES "^4.+")
	message(ERROR "!!! unsupported compiler version, require >= GCC 5+, but only found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Generate debug library name with a postfix.")
include(cmake/macros.cmake)
find_version(_version _soversion)

option(${project_prefix}_ENABLE_ADDRESS_SANITIZER    "enable address sanitizer (default=no)" OFF)

include(cmake/gcc.cmake)
include(cmake/msvc.cmake)
include_directories(include)
include_directories(${CMAKE_BINARY_DIR}/include)

message("-- version === ${_version} / ${_soversion}")

set(zmq_include "$<BUILD_INTERFACE:${${project_prefix}_ZMQ_PATH}/include>")
set(cppzmq_include "$<BUILD_INTERFACE:${${project_prefix}_CPPZMQ_PATH}>")
set(protobuf_include "$<BUILD_INTERFACE:${${project_prefix}_PROTOBUF_PATH}/src>")
set(protobuf_compiler $<TARGET_FILE:protoc>)
set(protobuf_path $<TARGET_FILE_DIR:protoc>)

add_custom_command(
		OUTPUT
		${CMAKE_BINARY_DIR}/include/ufecpp/ufeapi.pb.h
		${CMAKE_BINARY_DIR}/include/ufecpp/ufeapi.pb.cc
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/include/ufecpp
		COMMAND ${CMAKE_COMMAND} -E env ${_ld_library_path}=${protobuf_path} ${protobuf_compiler} --cpp_out=${CMAKE_BINARY_DIR}/include/ufecpp ufeapi.proto
		MAIN_DEPENDENCY include/ufeapi/ufeapi.proto
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/include/ufeapi)

add_library(ufeedclient SHARED
		include/ufecpp/ufecppversion.h
		include/ufecpp/ufecppdll.h
		include/ufecpp/ufeedclient.hpp
		src/ufeedclient/ufeedclient.cpp
		${CMAKE_BINARY_DIR}/include/ufecpp/ufeapi.pb.cc ${CMAKE_BINARY_DIR}/include/ufecpp/ufeapi.pb.h
		include/ufecpp/ufemessage.hpp
		include/ufecpp/ufeconsts.hpp
		include/ufecpp/ufeconfiguration.hpp
		include/ufecpp/ufeexception.hpp
		include/ufecpp/ufe_cpp_fields_fix40.hpp
		include/ufecpp/ufe_cpp_fields_fix41.hpp
		include/ufecpp/ufe_cpp_fields_fix42.hpp
		include/ufecpp/ufe_cpp_fields_fix43.hpp
		include/ufecpp/ufe_cpp_fields_fix44.hpp
		include/ufecpp/ufe_cpp_fields_fix50.hpp
		include/ufecpp/ufe_cpp_fields_fix50sp1.hpp
		include/ufecpp/ufe_cpp_fields_fix50sp2.hpp
		include/ufecpp/deprecated/ufeapi.hpp
	)
target_include_directories(ufeedclient PRIVATE
		${protobuf_include}
		${zmq_include}
		${cppzmq_include}
		)
target_link_libraries(ufeedclient
		PRIVATE
			$<BUILD_INTERFACE:libzmq-static>
		PUBLIC
			libprotobuf-lite
		)
find_version(ver sover)
target_link_set_version(ufeedclient ${sover})

add_executable(genconsts src/genconsts/genconsts.cpp)
target_include_directories(genconsts PRIVATE 3rdparty/cxxopts/include)

# deprecated

# tests
enable_testing()
add_executable(ufeedclient_test src/test/ufeedclient_test.cpp)
target_include_directories(ufeedclient_test PRIVATE
	${protobuf_include}
	${zmq_include}
	${cppzmq_include}
	)
target_link_libraries(ufeedclient_test PRIVATE
		$<BUILD_INTERFACE:gtest_main>
	PUBLIC
		ufeedclient
		uuid
	)

add_test(NAME ufeedclient_test COMMAND ${CMAKE_COMMAND} -E env $<TARGET_FILE:ufeedclient_test>)


if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/.install" CACHE PATH "install path" FORCE)
endif()
message("-- installing to ${CMAKE_INSTALL_PREFIX}")
install(TARGETS ufeedclient RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
install(TARGETS libprotobuf-lite RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
install(DIRECTORY include/ufecpp DESTINATION include FILES_MATCHING PATTERN *.h PATTERN *.hpp)
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ufecpp DESTINATION include FILES_MATCHING PATTERN *.h PATTERN *.hpp)
install(DIRECTORY 3rdparty/protobuf/src/google/protobuf/ DESTINATION include/google/protobuf/ FILES_MATCHING PATTERN *.h PATTERN *.inc)
install(FILES src/samples/CMakeLists.txt DESTINATION samples/)
install(FILES src/samples/sample0.cpp DESTINATION samples/)

