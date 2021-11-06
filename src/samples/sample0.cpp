#include <cassert>
#include <memory>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <ufecpp/ufeedclient.hpp>
#include <ufecpp/ufe_cpp_fields_fix50sp2.hpp>

using namespace std;
using namespace std::chrono_literals;
using namespace UFECPP;
using namespace UFE;

struct Listener: public UFEedClient::Listener
{
	vector<UFEMessagePtr> _sub_msgs, _res_msgs, _rep_msgs;
	string _login, _password;
	mutex _auth_lock, _subs_lock;
	condition_variable _auth_cond, _subs_cond;
	bool _authenticated { false };

	void subscription_message_received(const UFEMessagePtr &message) override
	{
		unique_lock lk(_subs_lock);
		_sub_msgs.push_back(message->clone());
		_subs_cond.notify_one();
	}
	void responder_message_received(const UFEMessagePtr &message) override
	{
		_rep_msgs.push_back(message->clone());
	}
	void response_message_received(const UFEMessagePtr &message) override
	{
		_res_msgs.push_back(message->clone());
	}
	bool authenticate_requested(const std::string &user, const std::string &password) override
	{
		unique_lock lk(_auth_lock);
		_login = user;
		_password = password;
		_authenticated = true;
		_auth_cond.notify_one();
		return true;
	}
	bool zeromq_error_happened(int error) override
	{
		cerr << "ZeroMQ error happened: " << error << endl;
		return false;
	}
	bool error_happened(const std::string &error, const std::exception &exception) override
	{
		cerr << "Error happened: " << error << endl;
		return false;
	}
};

int main(int argc, char** argv)
{
	Listener listener;
	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
													.max_io_threads(2)
													.poll_interval_ms(1)
													.subscriber(SUBSCRIBER_DEFAULT)
													.responder_topic("ufegw-requester")
	, &listener);

	_uc->start(false);

	// logon
	auto login = _uc->create_message();
	login
		.set_long_name("login")
		.set_type(WireMessage::st_system)
		.set_service_id(UFE_CMD_LOGIN)
		.add_field(UFE_CMD, UFE_CMD_LOGIN)
		.add_field(UFE_LOGIN_ID, "webuser")
		.add_field(UFE_LOGIN_PW, "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8");
	auto response = _uc->request(move(login));
	assert(1 == listener._res_msgs.size());
	assert(nullptr == listener._res_msgs[0]->find_field(UFE_SESSION_TOKEN));
	auto opt_tkn{ listener._res_msgs[0]->find_field_value<std::string>(UFE_SESSION_TOKEN) };
	assert(opt_tkn.has_value());
	assert(!opt_tkn->empty());

	// service list request
	auto svr_list_req = _uc->create_message();
	svr_list_req.set_long_name("service_list")
		.set_type(WireMessage::st_system)
		.add_field(UFE_CMD, Status{ ._status = UFE_CMD_SERVICE_LIST});
	response = _uc->request(move(svr_list_req));
	assert(1 == response->groups().size());

	// subscription check
	{
		unique_lock lk(listener._subs_lock);
		listener._subs_cond.wait_for(lk, 15s, [&listener]{ return !listener._sub_msgs.empty(); });
		assert(!listener._sub_msgs.empty());
	}

	// authentication check
	// - this requires manual run of f8ptest initiator to connect to one of the auth session in ufegw
	{
		cerr << "waiting for auth..." << endl;
		unique_lock lk(listener._auth_lock);
		listener._auth_cond.wait_for(lk, 100s, [&listener]{ return listener._authenticated; });
		assert(listener._authenticated);
		cerr << "authenticated == " << listener._authenticated << endl;
	}

	// NOS creation
	using namespace FIX50SP2::Field;
	auto nos = _uc->create_message();
	nos.set_long_name("NewOrderSingle")
		.set_type(WireMessage::st_fixmsg)
		.set_service_id(1)
		.set_name(MsgType::NEWORDERSINGLE)
		.add_field(ClOrdID::tag, "123")
		.add_field(TransactTime::tag, std::chrono::system_clock::now())
		.add_field(ExecInst::tag, ExecInst::ALL_OR_NONE)
		.add_field(OrdType::tag, OrdType::LIMIT)
		.add_field(Side::tag, Side::BUY)
		;

	_uc->stop();
	_uc.reset();
}
