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

#ifndef UFECPPBINDING_UFEEDCLIENT_HPP
#define UFECPPBINDING_UFEEDCLIENT_HPP

#include <ufecpp/ufecppdll.h>
#include <ufecpp/ufecppversion.h>
#include <ufecpp/ufeconsts.hpp>
#include <ufecpp/ufeconfiguration.hpp>
#include <ufecpp/ufemessage.hpp>

namespace UFECPP
{
	/**
	 * UFEed Client class - main access point to UFEGW
	 * \example
	 * 	struct Listener: public UFEedClient::Listener {...} listener;
	 * 	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
	 * 		.max_io_threads(2)
	 * 		.poll_interval_ms(1)
	 * 		.subscriber(SUBSCRIBER_DEFAULT)
	 * 		.responder_topic("ufegw-requester")
	 * 	, &listener);
	 * 	_uc->connect();
	 * 	// logon
	 * 	auto login = _uc->create_message("login", WireMessage::st_system, UFE_CMD_LOGIN);
	 * 	login->add_field(UFE_CMD, UFE_CMD_LOGIN)
	 * 		.add_field(UFE_LOGIN_ID, "abcdef")
	 * 		.add_field(UFE_LOGIN_PW, "1e884898da28047151d0e56f8df6292773603d0d6aabbdd62a11ef721d154244");
	 * 	auto response = _uc->request(move(login));
	 * 	...
	 * 	_uc->stop();
	 * 	_uc.reset();
	 */
	class UFEedClient
	{
		struct Impl;
		std::unique_ptr<Impl> _impl;

	public:
		/**
		 * UFEed Client events interface.
		 * Consumer must derive a class from Listener and implement virtual methods
		 */
		struct Listener
		{
			/**
			 * Called when subscription message received
			 * \param message received subscription message
			 */
			virtual void subscription_message_received(const UFEMessagePtr& message) = 0;
			/**
			 * Called when responder message received
			 * \param message received responder message
			 */
			virtual void responder_message_received(const UFEMessagePtr& message) = 0;
			/**
			 * Called when response message received
			 * \param message received response message
			 */
			virtual void response_message_received(const UFEMessagePtr& message) = 0;
			/**
			 * Called when authentication is requested
			 * \param user user to check
			 * \param password user password to check
			 * \return true for successful authentication, otherwise false
			 */
			virtual bool authenticate_requested(const std::string& user, const std::string& password) = 0;
			/**
			 * Called when ZeroMQ error happened
			 * \param error ZMQ error code
			 * \return true to continue, false to stop processing loop
			 */
			virtual bool zeromq_error_happened(int error) = 0;
			/**
			 * Called when error happened
			 * \param error error message
			 * \param exception exception happened
			 * \return true to continue, false to stop processing loop
			 */
			virtual bool error_happened(const std::string& error, const std::exception& exception) = 0;
		};

		/**
		 * UFEed Client ctor
		 * \param config UFEedConfiguration to configure UFEed Client
		 * \param listener listener based class to receive UFEed Client events
		 */
		UFECPPAPI explicit UFEedClient(const UFEedConfiguration& config, Listener* listener);
		/**
		 * Dtor
		 * Stops UFEed Client if it was started and frees allocated resources
		 */
		UFECPPAPI ~UFEedClient();
		/**
		 * Starts UFEed Client. When started in synchronous mode (wait = true)
		 * it does not return until stop() is called from a different thread.
		 * \param wait true for synchronous call, false for asynchronous
		 */
		UFECPPAPI void start(bool wait = false);
		/**
		 * Stops UFEed Client
		 */
		UFECPPAPI void stop();
		/**
		 * Creates UFEMessage buidler
		 * \return message builder. \see UFEMessage::Builder
		 */
		UFECPPAPI UFEMessage::Builder create_message();
		/**
		 * Creates UFEMessage builder
		 * \param wmwa WireMessage uinque ptr to take ownership from, i.e. using std::move().
		 * Ownership of WireMessage IS taken. \see WireMessage
		 * \return message builder. \see UFEMessage::Builder
		 */
		UFECPPAPI UFEMessage::Builder create_message(WireMessageWithArena&& wmwa);
		/**
		 * Synchronously sends request to UFEGW and waits for UFE response
		 * \param request request to send. Ownership of UFEMessage unique ptr IS taken. \see UFEMessage
		 * \return
		 */
		UFECPPAPI UFEMessagePtr request(UFEMessage::Builder&& request);
		/**
		 * Send message to responder channel. MUST be call in listener thread context
		 * \param msg request to send. Ownership of UFEMessage unique ptr IS taken. \see UFEMessage
		 */
		UFECPPAPI void respond(UFEMessage::Builder&& msg);
	};

	/// UFEed Client shared pointer type
	using UFEedClientPtr = std::shared_ptr<UFEedClient>;
}

#endif //UFECPPBINDING_UFEEDCLIENT_HPP
