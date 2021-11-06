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

#ifndef UFECPPBINDING_UFEEXCEPTION_HPP
#define UFECPPBINDING_UFEEXCEPTION_HPP

#include <exception>
#include <sstream>

#include <ufecpp/ufecppdll.h>
#include <ufecpp/ufecppversion.h>

namespace UFECPP
{
	/**
	 * UFE base exception class
	 */
	class UFEException: public std::exception
	{
		std::string _message;

		template<typename T, typename... Args>
		void inner_format(std::ostream& oss, T val, Args&&... args) const
		{ oss << val; inner_format(oss, args...); }
		static void inner_format(std::ostream& oss) {}

	public:
		/**
		 * Ctor
		 * \param error error message to record
		 */
		explicit UFEException(std::string error)
			: _message(std::move(error))
		{}

		/**
		 * Formatting ctor
		 * \tparam Args formatting arguments types
		 * \param args formatting arguments that have to have operator<< implemented
		 */
		template<typename... Args>
		explicit UFEException(Args&&... args)
		{
			std::ostringstream oss;
			inner_format(oss, std::forward<Args>(args)...);
			_message = oss.str();
		}

		/**
		 * std::exception::what() override
		 * \return exception message pased to ctor
		 */
		const char* what() const noexcept override { return _message.c_str(); }
	};
}

#endif //UFECPPBINDING_UFEEXCEPTION_HPP
