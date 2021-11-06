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
// UFECPPAPI required only non-inlined publics, public/private statics that exposed via inline
// functions - or the entire class could be marked with UFECPPAPI.
//-------------------------------------------------------------------------------------------------
#ifndef UFEGW_DLL_H_INCLUDED_
#define UFEGW_DLL_H_INCLUDED_

#if defined(_MSC_VER)
#if defined(BUILD_UFECPPAPI)
	    #define UFECPPAPI __declspec(dllexport)
    #else
	    #define UFECPPAPI __declspec(dllimport)
        #ifndef UFEGW_NO_AUTOLINK
            #ifdef _DEBUG
                #define UFEGW_LIB_SUFFIX "d.lib"
            #else
                #define UFEGW_LIB_SUFFIX ".lib"
            #endif
            #pragma comment(lib, "ufecpp" UFEGW_LIB_SUFFIX)
        #endif
    #endif
#else
#define UFECPPAPI
#endif

#endif // UFEGW_DLL_H_INCLUDED_
