//-------------------------------------------------------------------------------------------------
/*
$Id: $
    ____                      __      ____
   /\  _`\   __             /'_ `\   /\  _`\
   \ \ \L\_\/\_\    __  _  /\ \L\ \  \ \ \L\ \ _ __    ___
    \ \  _\/\/\ \  /\ \/'\ \/_> _ <_  \ \ ,__//\`'__\ / __`\
     \ \ \/  \ \ \ \/>  </   /\ \L\ \  \ \ \/ \ \ \/ /\ \L\ \
      \ \_\   \ \_\ /\_/\_\  \ \____/   \ \_\  \ \_\ \ \____/
       \/_/    \/_/ \//\/_/   \/___/     \/_/   \/_/  \/___/

               Fix8Pro FIX Engine and Framework

Copyright (C) 2010-19 Fix8 Market Technologies Pty Ltd (ABN 29 167 027 198)
All Rights Reserved. [http://www.fix8mt.com] <heretohelp@fix8mt.com>

THIS FILE IS PROPRIETARY AND  CONFIDENTIAL. NO PART OF THIS FILE MAY BE REPRODUCED,  STORED
IN A RETRIEVAL SYSTEM,  OR TRANSMITTED, IN ANY FORM OR ANY MEANS,  ELECTRONIC, PHOTOSTATIC,
RECORDED OR OTHERWISE, WITHOUT THE PRIOR AND  EXPRESS WRITTEN  PERMISSION  OF  FIX8  MARKET
TECHNOLOGIES PTY LTD.

*/
//-------------------------------------------------------------------------------------------------
#ifndef UFECPP_VERSION_H_INCLUDED_
#define UFECPP_VERSION_H_INCLUDED_

//-------------------------------------------------------------------------------------------------
// FIX8 version numbers - must be numbers, not strings
//-------------------------------------------------------------------------------------------------

/* Major version number */
#define UFECPP_MAJOR_VERSION_NUM 21

/* Minor version number */
#define UFECPP_MINOR_VERSION_NUM 9

/* Patch number */
#define UFECPP_PATCH_VERSION_NUM 0

/* Semantic number */
#define UFECPP_SEMANTIC_VERSION_MULT 100
#define UFECPP_SEMANTIC_VERSION_NUM ( UFECPP_MAJOR_VERSION_NUM * UFECPP_SEMANTIC_VERSION_MULT + UFECPP_MINOR_VERSION_NUM )

/* Age number - shows back compatible versions */
#define UFECPP_AGE_VERSION_NUM 0

//-------------------------------------------------------------------------------------------------
// Do not edit below this line
//-------------------------------------------------------------------------------------------------
#define UFECPP_STRINGIZE_INPLACE(x) #x
#define UFECPP_STRINGIZE(x) UFECPP_STRINGIZE_INPLACE(x)

#define UFECPP_MAKE_VERSION(maj,min,pat) (((maj * 100 + min) * 100) + pat)

#if UFECPP_MINOR_VERSION_NUM < 10
#define UFECPP_ZERO_PREFIX 0
#else
#define UFECPP_ZERO_PREFIX
#endif

/* Package version */
#if UFECPP_PATCH_VERSION_NUM > 0
#define UFECPP_PACKAGE_VERSION UFECPP_STRINGIZE(UFECPP_MAJOR_VERSION_NUM) "." UFECPP_STRINGIZE(UFECPP_ZERO_PREFIX) UFECPP_STRINGIZE(UFECPP_MINOR_VERSION_NUM) "." UFECPP_STRINGIZE(UFECPP_PATCH_VERSION_NUM)
#else
#define UFECPP_PACKAGE_VERSION UFECPP_STRINGIZE(UFECPP_MAJOR_VERSION_NUM) "." UFECPP_STRINGIZE(UFECPP_ZERO_PREFIX) UFECPP_STRINGIZE(UFECPP_MINOR_VERSION_NUM)
#endif

/* Semantic package version */
#define UFECPP_SEMANTIC_PACKAGE_VERSION UFECPP_STRINGIZE(UFECPP_SEMANTIC_VERSION_NUM) "." UFECPP_STRINGIZE(UFECPP_PATCH_VERSION_NUM) "." UFECPP_STRINGIZE(UFECPP_AGE_VERSION_NUM)

/* Package name */
#define UFECPP_PACKAGE_NAME FIX8PRO_PACKAGE

/* Encoded version */
#define UFECPP_MAGIC_NUM UFECPP_MAKE_VERSION(UFECPP_MAJOR_VERSION_NUM, UFECPP_MINOR_VERSION_NUM, UFECPP_PATCH_VERSION_NUM)

#define UFECPP_INPLACE2_(x,y) x##y
#define UFECPP_INPLACE2(x,y) UFECPP_INPLACE2_(x,y)

/* C++ namespace alias */
#define UFECPP UFECPP_INPLACE2(UFECPP_INPLACE2(UFECPP_INPLACE2(ufecpp_, UFECPP_MAJOR_VERSION_NUM), UFECPP_ZERO_PREFIX), UFECPP_MINOR_VERSION_NUM)

/* deprecated UFECPP version */
#define UFECPP_DEPRECATED ufecpp_2003
//-------------------------------------------------------------------------------------------------
#endif // UFECPP_VERSION_H_INCLUDED_
