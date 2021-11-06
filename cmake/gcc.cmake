#############################################################################################
# $Id: $
#   ____                      __      ____
#  /\  _`\   __             /'_ `\   /\  _`\
#  \ \ \L\_\/\_\    __  _  /\ \L\ \  \ \ \L\ \ _ __    ___
#   \ \  _\/\/\ \  /\ \/'\ \/_> _ <_  \ \ ,__//\`'__\ / __`\
#    \ \ \/  \ \ \ \/>  </   /\ \L\ \  \ \ \/ \ \ \/ /\ \L\ \
#     \ \_\   \ \_\ /\_/\_\  \ \____/   \ \_\  \ \_\ \ \____/
#      \/_/    \/_/ \//\/_/   \/___/     \/_/   \/_/  \/___/
#
#              Fix8Pro FIX Engine and Framework
#
# Copyright (C) 2010-19 Fix8 Market Technologies Pty Ltd (ABN 29 167 027 198)
# All Rights Reserved. [http://www.fix8mt.com] <heretohelp@fix8mt.com>
#
# THIS FILE IS PROPRIETARY AND  CONFIDENTIAL. NO PART OF THIS FILE MAY BE REPRODUCED,  STORED
# IN A RETRIEVAL SYSTEM,  OR TRANSMITTED, IN ANY FORM OR ANY MEANS,  ELECTRONIC, PHOTOSTATIC,
# RECORDED OR OTHERWISE, WITHOUT THE PRIOR AND  EXPRESS WRITTEN  PERMISSION  OF  FIX8  MARKET
# TECHNOLOGIES PTY LTD.
#
#############################################################################################
if (UNIX)
	add_definitions(-DFIX8PRO_REQUIRE_CMAKE_BUILD=1)
	set(CMAKE_C_FLAGS "-fPIC")
	set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
	set(CMAKE_C_FLAGS_RELEASE "-O3")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -ggdb")
	set(CMAKE_CXX_FLAGS "-fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG -ggdb")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG -ggdb")
	if (NOT "${${project_prefix}_ARCH}" STREQUAL "")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -march=${${project_prefix}_ARCH}")
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=${${project_prefix}_ARCH}")
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -march=${${project_prefix}_ARCH}")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -march=${${project_prefix}_ARCH}")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=${${project_prefix}_ARCH}")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -march=${${project_prefix}_ARCH}")
	endif()
	if (${${project_prefix}_ENABLE_ADDRESS_SANITIZER})
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fsanitize=address")
		message("-- address sanitizer is ON")
	endif()
	set(_ld_library_path "LD_LIBRARY_PATH")
	set(_ld_library_path_sep ":")
endif()
