#include <atomic>
#include <thread>
#include <array>
#include <utility>
#include <zmq.hpp>

#ifdef _MSC_VER
# include <Rpc.h> // for UUID
using uuid = UUID;
#else
# include <uuid/uuid.h>
struct uuid { uuid_t _id; };
#endif


#include <ufecpp/ufeedclient.hpp>
#include <ufecpp/ufeexception.hpp>

namespace UFECPP
{
	using namespace std;

	class spin_lock
	{
		atomic_flag _sl = ATOMIC_FLAG_INIT;
	public:
		class scoped_lock
		{
			spin_lock& _lk;
		public:
			explicit scoped_lock(spin_lock& lock): _lk(lock) { _lk.lock(); }
			~scoped_lock() { _lk.unlock(); }
		};

		void lock() { while (!try_lock()); }
		bool try_lock() { return !_sl.test_and_set(memory_order_acquire); }
		void unlock() { _sl.clear(memory_order_release); }
	};

	struct UFEedClient::Impl
	{
	private:
		UFEedConfiguration _cs;
		UFEedClient::Listener* _listener{ nullptr };

		zmq::context_t _context;
		zmq::socket_t _req_socket;
		zmq::socket_t _rep_socket;
		zmq::socket_t _sub_socket;

		Uuid _session_id;
		atomic<bool> _started{ false }, _stop_requested{ false };
		uint32_t _responderSeq{ 0 };
		unique_ptr<thread> _worker_thread;

		spin_lock _rep_socket_lock;
		spin_lock _req_socket_lock;

		static string new_guid()
		{
			uuid id{};
#ifdef _MSC_VER
			UuidCreate(&id);
			RPC_CSTR buf;
			UuidToStringA(&_uuid, &buf);
			string result(reinterpret_cast<const char *>(buf));
			RpcStringFreeA(&buf);
			return result;
#else
			uuid_generate(id._id);
			char result[64];
			uuid_unparse(id._id, result);
			return result;
#endif
		}

		void process_respond_message(UFEMessagePtr&& msg)
		{
			auto rmsg{create_message() };
			rmsg.set_service_id(msg->service_id()).set_subservice_id(msg->subservice_id());

			Status response_code{ ._status = UFE_OK };
			ostringstream response_text;
			string req_token;
			if (auto rqtkn{ msg->find_field(UFE_REQUEST_TOKEN) }; rqtkn)
				req_token = rqtkn->sval();

			const UFEField * cmd{ nullptr };
			switch (msg->wire_message()->type())
			{
				case WireMessage::st_fixmsg:
					break;
				case WireMessage::st_system:
					cmd = msg->find_field(UFE_CMD);
					if (!cmd)
					{
						response_text << "command not present " << msg->wire_message()->type() << " on topic=" << _cs.responder_topic();
						response_code._status = NO_CMD;
					}
					else
					{
						auto cmdToken = cmd->ival();
						if(cmdToken == UFE_CMD_AUTHENTICATE)
						{
							// authentication response
							auto usr_token = msg->find_field(553); // user_name
							string user_name, password;
							if (usr_token)
								user_name = usr_token->sval();
							auto pw_token = msg->find_field(554); // Password
							if (pw_token)
								password = pw_token->sval();
							auto accepted{ true };
							if (_listener)
								accepted = _listener->authenticate_requested(user_name, password);
							if (accepted)
							{
								response_text << "authentication was successful";
								response_code._status = LOGIN_ACCEPTED;
							}
							else
							{
								response_text << "user or password is incorrect";
								response_code._status = UNKNOWN_USER;
							}
						}
						else
						{
							response_code._status = NO_CMD;
						}
						rmsg.add_field(UFE_CMD_RESPONSE, cmdToken, UFEField::fl_system);
					}
					break;
				default:
					response_text << "unknown message type or command " << msg->wire_message()->type() << " on topic=" << _cs.responder_topic();
					response_code._status = UNKNOWN_TYPE;
					break;
			}
			// Generate response and send
			rmsg.set_seq(++_responderSeq)
				.add_field(UFE_RESPONSE_CODE, response_code, UFEField::fl_system)
				.add_field(COMMON_REFSEQNUM, rmsg.seq(), UFEField::fl_system);
			if (!req_token.empty())	// if request token received, echo back
				rmsg.add_field(UFE_REQUEST_TOKEN, move(req_token), UFEField::fl_system);
			switch (response_code._status)
			{
				case UFE_OK:
				case LOGIN_ACCEPTED:
					rmsg.add_field(UFE_RESPONSE_TOKEN, new_guid(), UFEField::fl_system)
						.set_type(WireMessage::st_response);
					break;
				default: // is an error
					rmsg.set_type(WireMessage::st_error);
					break;
			}

			auto response_text_str{ response_text.str() };
			if (!response_text_str.empty()) // both errors and success can pass a text msg
				rmsg.add_field(COMMON_TEXT, move(response_text_str), UFEField::fl_system);
			respond(move(rmsg));
		}

		void worker()
		{
			_sub_socket.connect(_cs.subscriber());
			_sub_socket.setsockopt(ZMQ_SUBSCRIBE, _cs.subscriber_topic().c_str(), _cs.subscriber_topic().length());
			if (!_cs.responder().empty())
				_rep_socket.bind(_cs.responder());
			array<zmq::pollitem_t, 2> pitems{
				zmq::pollitem_t{ _sub_socket, 0, ZMQ_POLLIN },
				zmq::pollitem_t{ _rep_socket, 0, ZMQ_POLLIN } };

			while(!_stop_requested)
			{
				try
				{
					if (zmq::poll(&pitems[0], pitems.size(), _cs.poll_interval_ms()) > 0)
					{
						if (pitems[0].revents && ZMQ_POLLIN)
						{
							// i==0, subscriber message
							auto msg0 = recv(_sub_socket);
							if (_listener)
								_listener->subscription_message_received(msg0);
						}
						// i==1, responder message
						if (pitems[1].revents && ZMQ_POLLIN)
						{
							UFEMessagePtr msg1;
							{
								spin_lock::scoped_lock lk(_rep_socket_lock);
								msg1 = recv(_rep_socket);
							}
							if (_listener)
								_listener->responder_message_received(msg1);
							process_respond_message(move(msg1));
						}
					}
				}
				catch (const zmq::error_t &zex)
				{
					if (zex.num() == EAGAIN)
						continue;
					if (_listener)
						_stop_requested = !_listener->zeromq_error_happened(zex.num());
				}
				catch(const exception& ex)
				{
					if (_listener)
						_stop_requested = !_listener->error_happened(ex.what(), ex);
				}
			}
		}

		static size_t send(zmq::socket_t& socket, const string& topic, const WireMessage* msg)
		{
			string serialised_data;
			if (!msg->SerializeToString(&serialised_data))
				throw UFEException("Cannot serialize request");

			auto ret1 = socket.send(zmq::const_buffer(topic.c_str(), topic.length()), zmq::send_flags::sndmore);
			assert(ret1.has_value() && ret1.value() > 0);
			auto ret2 = socket.send(zmq::const_buffer(serialised_data.c_str(), serialised_data.length()), zmq::send_flags::none);
			assert(ret2.has_value() && ret2.value() > 0);
			return ret2.has_value() ? ret2.value() : 0;
		}

		UFEMessagePtr recv(zmq::socket_t& socket)
		{
			zmq::message_t msg1, msg2;
			auto ret = socket.recv(msg1, zmq::recv_flags::none);
			//assert(!ret); //E_EGAIN only possible with zmq::recv_flags::dontwait

			int64_t more { 1 };
			size_t more_size { sizeof(more) };
			socket.getsockopt(ZMQ_RCVMORE, &more, &more_size);
			if (!more)
				throw UFEException("Message has 1 frames but 2 is expected");
			ret = socket.recv(msg2, zmq::recv_flags::none);
			//assert(!ret); //E_EGAIN only possible with zmq::recv_flags::dontwait

			WireMessageWithArena wmwa;
			wmwa._arena = std::make_shared<gpb::Arena>();
			wmwa._wm = gpb::Arena::CreateMessage<WireMessage>(wmwa._arena.get());
			wmwa._wm->ParseFromArray(msg2.data(), msg2.size());
			return create_message(move(wmwa)).build(); // make_shared doesnt work since ctor is private
		}

		UFEMessagePtr send_request_recv_response(UFEMessage::Builder&& request)
		{
			spin_lock::scoped_lock lk(_req_socket_lock);
			send(_req_socket, _cs.requester_topic(), request.wire_message());
			return recv(_req_socket);
		}

	public:
		explicit Impl(UFEedConfiguration config, UFEedClient::Listener* listener)
			: _cs(move(config))
			, _listener(listener)
			, _context(_cs.max_io_threads())
			, _req_socket(_context, zmq::socket_type::req)
			, _rep_socket(_context, zmq::socket_type::rep)
			, _sub_socket(_context, zmq::socket_type::sub)
		{
		}

		~Impl()
		{
			stop();
			if (!_session_id._uuid.empty())
			{
				auto logout = create_message();
				logout.set_long_name("logout")
					.set_type(WireMessage::st_system)
					.set_service_id(UFE_CMD_LOGOUT)
					.add_field(UFE_CMD, UFE_CMD_LOGOUT);
				request(move(logout));
				_session_id._uuid.clear();
				_sub_socket.close();
				_rep_socket.close();
				_req_socket.close();
				_context.close();
			}
		}

		void start(bool wait)
		{
			if (_started)
				return;
			_started = true;
			_stop_requested = false;
			_req_socket.connect(_cs.requester());
			if (wait)
				worker();
			else
				_worker_thread = make_unique<thread>([this](){ worker(); });
		}

		void stop()
		{
			if (!_started)
				return;
			_stop_requested = true;
			if (_worker_thread)
				_worker_thread->join();
			_started = false;
		}

		UFEMessage::Builder create_message(WireMessageWithArena&& wmwa)
		{
			return UFEMessage::Builder(move(wmwa));
		}

		UFEMessage::Builder create_message()
		{
			WireMessageWithArena wmwa;
			wmwa._arena = std::make_shared<gpb::Arena>();
			wmwa._wm = gpb::Arena::CreateMessage<WireMessage>(wmwa._arena.get());
			return create_message(move(wmwa));
		}

		UFEMessagePtr request(UFEMessage::Builder&& request)
		{
			UFEMessagePtr response;
			// have we already logged in?
			if (!_session_id._uuid.empty())
			{
				request.add_field(UFE_SESSION_TOKEN, _session_id, UFEField::fl_system);
				response = send_request_recv_response(move(request));
			}
			// well ok, is this a login attempt?
			else if (request.service_id() == UFE_CMD_LOGIN)
			{
				response = send_request_recv_response(move(request));
				auto session_field = response->find_field(UFE_SESSION_TOKEN);
				if (session_field)
					_session_id._uuid = session_field->sval();
			}
			// neither?
			else
				throw UFEException("No session token found - you must log on before making a request");

			// send REP WireMessage to handler function
			if (_listener)
				_listener->response_message_received(response);
			return response;
		}

		void respond(UFEMessage::Builder&& builder)
		{
			spin_lock::scoped_lock lk(_rep_socket_lock);
			send(_rep_socket, _cs.responder_topic(), move(builder.wire_message()));
		}
	};


	UFEedClient::UFEedClient(const UFEedConfiguration& config, UFEedClient::Listener* listener)
		: _impl(make_unique<UFEedClient::Impl>(config, listener))
	{}

	UFEedClient::~UFEedClient() { _impl.reset(); }
	void UFEedClient::start(bool wait) { _impl->start(wait); }
	void UFEedClient::stop() { _impl->stop(); }
	UFEMessage::Builder UFEedClient::create_message() { return _impl->create_message(); }
	UFEMessage::Builder UFEedClient::create_message(WireMessageWithArena&& wmwa) { return _impl->create_message(move(wmwa)); }
	UFEMessagePtr UFEedClient::request(UFEMessage::Builder&& request) { return _impl->request(move(request)); }
	void UFEedClient::respond(UFEMessage::Builder&& msg) { _impl->respond(move(msg)); }

	static ostream& print_uuid(ostream& os, const string& uuid)
	{
#ifdef _MSC_VER
		RPC_CSTR buf;
		UuidToStringA(&uuid[0], &buf);
		string result(reinterpret_cast<const char *>(buf));
		RpcStringFreeA(&buf);
#else
		char result[64];
		uuid_unparse(reinterpret_cast<const unsigned char *>(&uuid[0]), result);
#endif
		return os << result;
	}

	ostream& UFEMessage::Builder::print_wm(ostream& os, const WireMessage& wm, int depth)
	{
		const string dspacer((1 + depth) * 3, ' ');
		const string sspacer(3, ' ');
		os << string(depth * 3, ' ') << "srvc_id=" << wm.service_id() << " subsrvc_id=" << wm.subservice_id() << " type=" << wm.type();
		if (!wm.name().empty())
			os << " msg=" << wm.name();
		if (!wm.longname().empty())
			os << " (" << wm.longname() << ')';
		os << " seq=" << wm.seq() << endl;

		for (const auto &pp : wm.fields())
		{
			os << dspacer << pp.tag();
			os << " (";
			switch (pp.location())
			{
				case UFEField::fl_header : os << "hdr" ; break;
				case UFEField::fl_body   : os << "body"; break;
				case UFEField::fl_trailer: os << "trl" ; break;
				case UFEField::fl_system : os << "sys" ; break;
				default:
					os << "unknown";
					break;
			 }
			os << "): ";
			switch (pp.type())
			{
				case UFEField::ft_status:
					os << sspacer << "status(" << pp.ival() << ')' << endl;
					break;
				case UFEField::ft_int:
					os << sspacer << pp.ival() << endl;
					break;
				case UFEField::ft_double:
					os << sspacer << pp.fval() << " (" << pp.ival() << ')' << endl;
					break;
				case UFEField::ft_bool:
					os << sspacer << (pp.bval() ? 'Y' : 'N') << endl;
					break;
				case UFEField::ft_char:
					os << sspacer << pp.sval()[0] << endl;
					break;
				case UFEField::ft_string:
					os << sspacer << pp.sval() << endl;
					break;
				case UFEField::ft_uuid:
					os << sspacer;
					print_uuid(os, pp.sval());
					os << endl;
					break;
				break;
				case UFEField::ft_msg:
					os << sspacer << pp.ival() << " elements, depth=" << depth << " ... " << endl;
					for (const auto& qq : pp.mval())
						print_wm(os, qq, depth + 1);
					break;
				case UFEField::ft_time:
				{
#if defined(__APPLE__) || defined(_MSC_VER)
					using this_time_point = chrono::time_point<chrono::system_clock>;
#else
					using this_time_point = chrono::time_point<chrono::high_resolution_clock>;
#endif
					const this_time_point val(chrono::duration_cast<this_time_point::duration>(chrono::nanoseconds(pp.ival())));
					const time_t valc(chrono::system_clock::to_time_t(val));
					os << sspacer << put_time(gmtime(&valc), "%F %T")
						<< '.' << chrono::duration_cast<chrono::nanoseconds>(val.time_since_epoch()).count() % nano::den << endl;
					break;
				}
				default:
					os << "Unknown field type:" << pp.type() << endl;
					break;
			 }
		}
		return os;
	}
}
