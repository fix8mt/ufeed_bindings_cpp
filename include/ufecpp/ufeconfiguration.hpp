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

#ifndef UFECPPBINDING_UFECONFIGURATION_H
#define UFECPPBINDING_UFECONFIGURATION_H

#include <string>
#include <ufecpp/ufecppdll.h>
#include <ufecpp/ufecppversion.h>
#include <ufecpp/ufeconsts.hpp>

namespace UFECPP
{
	/**
	 * UFEedClient configuration class
	 * \example
	 * 	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
	 * 		.max_io_threads(2)
	 * 		.poll_interval_ms(1)
	 * 		.subscriber(SUBSCRIBER_DEFAULT)
	 * 		.responder_topic("ufegw-requester")
	 * 	, &listener);
	 */
	class UFEedConfiguration
	{
		std::string _subscriber{ SUBSCRIBER_DEFAULT };
		std::string _requester{ REQUESTER_DEFAULT };
		std::string _publisher{ PUBLISHER_DEFAULT };
		std::string _responder{ RESPONDER_DEFAULT };
		std::string _subscriber_topic{ SUBSCRIBER_TOPIC_DEFAULT };
		std::string _requester_topic{ REQUESTER_TOPIC_DEFAULT };
		std::string _publisher_topic{ PUBLISHER_TOPIC_DEFAULT };
		std::string _responder_topic{ RESPONDER_TOPIC_DEFAULT };
		uint32_t _max_io_threads{ 1 };
		uint64_t _poll_interval_ms{ 10 };
		
	public:
		/**
		 * Subscriber endpoint, defaults to "tcp://127.0.0.1:55745"
		 * \return subscriber endpoint
		 */
		std::string subscriber() const { return _subscriber; }
		/**
		 * Sets subscriber endpoint
		 * \param subscriber subscribe endpoint in zmq endpoint format
		 * \return *this
		 */
		UFEedConfiguration& subscriber(std::string subscriber) { _subscriber = std::move(subscriber); return *this; }
		/**
		 * Requester endpoint, defaults to "tcp://127.0.0.1:55746"
		 * \return subscriber endpoint
		 */
		std::string requester() const { return _requester; }
		/**
		 * Sets requester endpoint
		 * \param requester requester endpoint in zmq endpoint format
		 * \return *this
		 */
		UFEedConfiguration& requester(std::string requester) { _requester = std::move(requester); return *this; }
		/**
		 * Publisher endpoint, defaults to "tcp://\*:55747"
		 * \return publisher endpoint
		 */
		std::string publisher() const { return _publisher; }
		/**
		 * Sets publisher endpoint
		 * \param publisher publisher endpoint in zmq endpoint format
		 * \return *this
		 */
		UFEedConfiguration& publisher(std::string publisher) { _publisher = std::move(publisher); return *this; }
		/**
		 * Responder endpoint, defaults to "tcp://\*:55748"
		 * \return responder endpoint
		 */
		std::string responder() const { return _responder; }
		/**
		 * Sets responder endpoint
		 * \param responder responder endpoint in zmq endpoint format
		 * \return *this
		 */
		UFEedConfiguration& responder(std::string responder) { _responder = std::move(responder); return *this; }
		/**
		 * Subscriber topic, defaults to "ufegw-publisher"
		 * \return subscriber topic
		 */
		std::string subscriber_topic() const { return _subscriber_topic; }
		/**
		 * Sets subscriber topic
		 * \param subscriber_topic subscriber topic that should match to UFEGW publisher topic
		 * \return *this
		 */
		UFEedConfiguration& subscriber_topic(std::string subscriber_topic) { _subscriber_topic = std::move(subscriber_topic); return *this; }
		/**
		 * Requester topic, defaults to "ufegw-responder"
		 * \return requester topic
		 */
		std::string requester_topic() const { return _requester_topic; }
		/**
		 * Sets requester topic
		 * \param requester_topic requester topic that should match to UFEGW responder topic
		 * \return *this
		 */
		UFEedConfiguration& requester_topic(std::string requester_topic) { _requester_topic = std::move(requester_topic); return *this; }
		/**
		 * Publisher topic, defaults to "ufeedclient-publisher"
		 * \return publisher topic
		 */
		std::string publisher_topic() const { return _publisher_topic; }
		/**
		 * Sets publisher topic
		 * \param publisher_topic publisher topic
		 * \return *this
		 */
		UFEedConfiguration& publisher_topic(std::string publisher_topic) { _publisher_topic = std::move(publisher_topic); return *this; }
		/**
		 * Responder topic, defaults to "ufeedclient-responder"
		 * \return responder topic
		 */
		std::string responder_topic() const { return _responder_topic; }
		/**
		 * Sets responder topic
		 * \param responder_topic responder topic
		 * \return *this
		 */
		UFEedConfiguration& responder_topic(std::string responder_topic) { _responder_topic = std::move(responder_topic); return *this; }
		/**
		 * Max zmq IO thread count, defaults to 1
		 * \return max zmq IO thread count
		 */
		uint32_t max_io_threads() const { return _max_io_threads; }
		/**
		 * Sets max zmq IO thread count
		 * \param max_io_threads max zmq IO thread count
		 * \return *this
		 */
		UFEedConfiguration& max_io_threads(uint32_t max_io_threads) { _max_io_threads = max_io_threads; return *this; }
		/**
		 * Poll interval in msecs, defaults to 10 ms
		 * \return poll interval in msecs
		 */
		uint64_t poll_interval_ms() const { return _poll_interval_ms; }
		/**
		 * Sets poll interval in msecs
		 * \param poll_interval_ms poll interval in msecs
		 * \return *this
		 */
		UFEedConfiguration& poll_interval_ms(uint64_t poll_interval_ms) { _poll_interval_ms = poll_interval_ms; return *this; }
	};
}

#endif //UFECPPBINDING_UFECONFIGURATION_H
