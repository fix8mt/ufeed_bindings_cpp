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
macro(find_version ver sover)
	file(READ include/ufecpp/ufecppversion.h verfile)
	string(REGEX REPLACE ".* UFECPP_SEMANTIC_VERSION_MULT ([0-9]+).*" "\\1" vermult ${verfile} )
	string(REGEX REPLACE ".* UFECPP_MAJOR_VERSION_NUM ([0-9]+).*" "\\1" vermajor ${verfile} )
	string(REGEX REPLACE ".* UFECPP_MINOR_VERSION_NUM ([0-9]+).*" "\\1" verminor ${verfile} )
	string(REGEX REPLACE ".* UFECPP_PATCH_VERSION_NUM ([0-9]+).*" "\\1" verpatch ${verfile} )
	string(REGEX REPLACE ".* UFECPP_AGE_VERSION_NUM ([0-9]+).*" "\\1" verage ${verfile} )
	MATH(EXPR versemantic "${vermajor} * ${vermult} + ${verminor}")
	set(${ver} "${vermajor}.${verminor}.${verpatch}")
	set(${sover} "${versemantic}.${verpatch}.${verage}")
endmacro()

macro(find_plugin_version file macro sover)
	file(READ include/UFECPP/UFECPPversion.h verfile0)
	file(READ ${file} verfile)
	string(REGEX REPLACE ".* UFECPP_SEMANTIC_VERSION_MULT ([0-9]+).*" "\\1" vermult ${verfile0} )
	string(REGEX REPLACE ".*F8_MAKE_VERSION.*([0-9]+),([0-9]+),([0-9]+).*" "\\1" vermajor ${verfile} )
	string(REGEX REPLACE ".*F8_MAKE_VERSION.*([0-9]+),([0-9]+),([0-9]+).*" "\\2" verminor ${verfile} )
	string(REGEX REPLACE ".*F8_MAKE_VERSION.*([0-9]+),([0-9]+),([0-9]+).*" "\\3" vermpatch ${verfile} )
	set(verage 0)
	MATH(EXPR versemantic "${vermajor} * ${vermult} + ${verminor}")
	set(${sover} "${versemantic}.${vermpatch}.${verage}")
endmacro()

macro(target_link_set_version target soversion)
	set_target_properties(${target} PROPERTIES VERSION ${soversion}) #VERSION ${_version} SOVERSION ${_soversion})
endmacro()

macro(add_install_target name)
	set(_install_targets ${_install_targets};${name})
endmacro()
