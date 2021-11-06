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
if (MSVC)
	add_definitions(-DFIX8PRO_REQUIRE_CMAKE_BUILD=1)
	add_definitions(-DWIN32=1 -D_CRT_SECURE_NO_WARNINGS -wd4251 -wd4503 -D_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING -D_SCL_SECURE_NO_WARNINGS) #-wd4250 -wd4675 -wd4099 -wd4181 -wd4396 -wd4275 -wd4996)

	set(_linker_common_flags "/OPT:REF /STACK:16777216")
	set(_c_cxx_release_common_flags "/MD /Zi /O2 /Ob2 /Oi /Ot /Oy /D NDEBUG /bigobj /EHsc")
	set(_c_cxx_debug_common_flags "/MDd /Zi /Od /Ob0 /D _DEBUG /bigobj /EHsc")

	set(CMAKE_C_FLAGS_DEBUG ${_c_cxx_debug_common_flags})
	set(CMAKE_CXX_FLAGS_DEBUG ${_c_cxx_debug_common_flags})
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_dbg}" CACHE STRING "1" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_dbg}" CACHE STRING "1" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "/INCREMENTAL:NO /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_dbg}" CACHE STRING "1" FORCE)

	set(CMAKE_C_FLAGS_RELWITHDEBINFO ${_c_cxx_release_common_flags})
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${_c_cxx_release_common_flags})
	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO /OPT:ICF /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO /OPT:ICF /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO /OPT:ICF /debug ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)

	set(CMAKE_C_FLAGS_RELEASE ${_c_cxx_release_common_flags})
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /OPT:ICF ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)
	set(CMAKE_MODULE_LINKER_FLAGS_RELEASE"/INCREMENTAL:NO /OPT:ICF ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /OPT:ICF ${_linker_common_flags} /LIBPATH:${_3rdparty_library_dir_opt}" CACHE STRING "1" FORCE)

	if (${project_prefix}_ENABLE_LTO)
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /GL")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /GL")
		set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/LTCG ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
		set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "/LTCG ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO}")
		set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/LTCG ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
		set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "/LTCG ${CMAKE_MODULE_LINKER_FLAGS_RELEASE}")
		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/LTCG ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
	endif()

	#string(REPLACE "Zm1000" "Zm500" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
	#string(REPLACE "Zm1000" "Zm500" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	set(_ld_library_path "PATH")
	set(_ld_library_path_sep ":")
endif()
