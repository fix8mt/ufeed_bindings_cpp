#include "gtest/gtest.h"

#include <memory>
#include <thread>
#include <condition_variable>
#include <ufecpp/ufeedclient.hpp>
#include <ufecpp/deprecated/ufeapi.hpp>

#include <ufecpp/ufe_cpp_fields_fix40.hpp>
#include <ufecpp/ufe_cpp_fields_fix41.hpp>
#include <ufecpp/ufe_cpp_fields_fix42.hpp>
#include <ufecpp/ufe_cpp_fields_fix43.hpp>
#include <ufecpp/ufe_cpp_fields_fix44.hpp>
#include <ufecpp/ufe_cpp_fields_fix50.hpp>
#include <ufecpp/ufe_cpp_fields_fix50sp1.hpp>
#include <ufecpp/ufe_cpp_fields_fix50sp2.hpp>

using namespace std;
using namespace std::chrono_literals;
using namespace UFECPP;
using namespace UFE;

UFECPP_DEPRECATED::UFEMessage create_deprecated_nos(TimePoint now)
{
	// NOS creation
	using namespace UFECPP::FIX50SP2::Field;
	using namespace UFECPP_DEPRECATED;
	static const auto fl_system = UFEField::fl_system;
	static const auto fl_body = UFEField::fl_body;

	auto wmsg = make_unique<WireMessage>();
	wmsg->set_longname("NewOrderSingle");
	wmsg->set_type(WireMessage::st_fixmsg);
	wmsg->set_service_id(1);
	wmsg->set_name(MsgType::NEWORDERSINGLE);
	add_string<fl_body>(wmsg.get(), ClOrdID::tag, "123");
	add_time<fl_body>(wmsg.get(), TransactTime::tag, now);
	//add_time<fl_body>(wmsg.get(), TransactTime::tag, chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch()).count());
	add_string<fl_body>(wmsg.get(), ExecInst::tag, ExecInst::ALL_OR_NONE);
	add_char<fl_body>(wmsg.get(), OrdType::tag, OrdType::LIMIT);
	add_char<fl_body>(wmsg.get(), Side::tag, Side::BUY);
	add_double<fl_body>(wmsg.get(), OrderQty::tag, 123.456, 3);
	add_double<fl_body>(wmsg.get(), Price::tag, 123.55999, 2);
	auto *grp = add_field(wmsg.get(), NoAllocs::tag, UFEField::ft_msg, fl_body);
	{
		auto *g1 = add_msg(grp, "NoAllocs", 1, WireMessage::st_fixmsg);
		{
			g1->set_longname("NoAllocs");
			add_string<fl_body>(g1, AllocAccount::tag, "ABC");
			add_double<fl_body>(g1, AllocQty::tag, 2.0, 2);
			auto *grp1 = add_field(g1, NoPartyIDs::tag, UFEField::ft_msg, fl_body);
			{
				auto *g11 = add_msg(grp1, "NoPartyIDs", 1, WireMessage::st_fixmsg);
				g11->set_longname("NoPartyIDs");
				add_string<fl_body>(g11, PartyID::tag, "abc");
			}
			grp1->set_ival(1);
		}
		auto *g2 = add_msg(grp, "NoAllocs", 2, WireMessage::st_fixmsg);
		{
			g2->set_longname("NoAllocs");
			add_string<fl_body>(g2, AllocAccount::tag, "CDE");
			add_double<fl_body>(g2, AllocQty::tag, 4.0, 2);
			auto *grp1 = add_field(g2, NoPartyIDs::tag, UFEField::ft_msg, fl_body);
			{
				auto *g11 = add_msg(grp1, "NoPartyIDs", 1, WireMessage::st_fixmsg);
				g11->set_longname("NoPartyIDs");
				add_string<fl_body>(g11, PartyID::tag, "cde");
			}
			grp1->set_ival(1);
		}
	}
	grp->set_ival(2);
	add_status<fl_system>(wmsg.get(), UFE_STATUS_CODE, UFE_OK);
	return UFECPP_DEPRECATED::UFEMessage(wmsg.release());
}

UFEMessage::Builder create_uptodate_nos(const UFEedClientPtr& uc, TimePoint now)
{
	// NOS creation
	using namespace FIX50SP2::Field;
	WireMessageGroup* grp{};
	auto nos = uc->create_message();
	nos.set_long_name("NewOrderSingle")
		.set_type(WireMessage::st_fixmsg)
		.set_service_id(1)
		.set_name(MsgType::NEWORDERSINGLE)
		.add_field(ClOrdID::tag, "123")
		.add_field(TransactTime::tag, now)
		.add_field(ExecInst::tag, ExecInst::ALL_OR_NONE)
		.add_field(OrdType::tag, OrdType::LIMIT)
		.add_field(Side::tag, Side::BUY)
		.add_field(OrderQty::tag, 123.456, 3)
		.add_field(Price::tag, 123.55999)
		.add_group(NoAllocs::tag, grp, [](UFEMessage::Builder& m, WireMessageGroup* g)
		{
			WireMessageGroup* grp2{};
			m.add_group_item(g)
				.set_long_name("NoAllocs"s)
				.set_name("NoAllocs")
				.set_type(WireMessage::st_fixmsg)
				.set_seq(1)
				.add_field(AllocAccount::tag, "ABC")
				.add_field(AllocQty::tag, 2.0)
				.add_group(NoPartyIDs::tag, grp2, [](UFEMessage::Builder& m1, WireMessageGroup* g1)
				{
					m1.add_group_item(g1)
						.set_long_name("NoPartyIDs")
						.set_name("NoPartyIDs")
						.set_type(WireMessage::st_fixmsg)
						.set_seq(1)
						.add_field(PartyID::tag, "abc");
				});
			m.add_group_item(g)
				.set_long_name("NoAllocs")
				.set_name("NoAllocs")
				.set_type(WireMessage::st_fixmsg)
				.set_seq(2)
				.add_field(AllocAccount::tag, "CDE"s)
				.add_field(AllocQty::tag, 4.0)
				.add_group(NoPartyIDs::tag, grp2, [](UFEMessage::Builder& m1, WireMessageGroup* g1)
				{
					m1.add_group_item(g1)
						.set_long_name("NoPartyIDs"s)
						.set_name("NoPartyIDs"s)
						.set_type(WireMessage::st_fixmsg)
						.set_seq(1)
						.add_field(PartyID::tag, "cde"s);
				});
		})
		.add_field(UFE_STATUS_CODE, Status{ ._status = UFE_OK }, UFEField::fl_system)
		;
	return move(nos);
}

string hex(const std::string& x)
{
	static const char d[17] = "0123456789ABCDEF";
	string ret(x.length() * 2, '\0');
	ret.reserve(x.length() * 2);
	auto idx = 0;
	for(auto& c: x)
	{
		ret[idx++] =  d[(c >> 4) & 0xF];
		ret[idx++] =  d[c & 0xF];
	}
	return ret;
}

TEST(ufeedclient, test_message)
{
	auto _uc = make_shared<UFEedClient>(UFEedConfiguration(), nullptr);
	auto nos = create_uptodate_nos(_uc, std::chrono::system_clock::now());
	cerr << nos << endl;

	ostringstream oss1, oss2;
	oss1 << nos;
	oss2 << *nos.build();
	EXPECT_EQ(oss1.str(), oss2.str());

	_uc.reset();
}

TEST(ufeapi_deprecated, test_message)
{
	auto _uc = make_shared<UFEedClient>(UFEedConfiguration(), nullptr);
	auto now = std::chrono::system_clock::now();
	auto nos_new = create_uptodate_nos(_uc, now).build();
	auto nos_old = create_deprecated_nos(now);

	using namespace FIX50SP2::Field;
	EXPECT_EQ(nos_new->long_name(), nos_old.get_longname());
	EXPECT_EQ(nos_new->type(), nos_old.get_type());
	EXPECT_EQ(nos_new->service_id(), nos_old.get_serviceid());
	EXPECT_EQ(nos_new->name(), nos_old.get_name());
	EXPECT_EQ(nos_new->fields().size() + nos_new->groups().size(), nos_old.get_wmsg()->fields().size());
	EXPECT_EQ(*nos_new->find_field_value<string>(ClOrdID::tag), nos_old.find_field(ClOrdID::tag)->sval());
	EXPECT_EQ(*nos_new->find_field_value<int64_t>(TransactTime::tag), nos_old.find_field(TransactTime::tag)->ival());
	EXPECT_EQ(chrono::duration_cast<chrono::nanoseconds>(nos_new->find_field_value<TimePoint>(TransactTime::tag)->time_since_epoch()).count(), nos_old.find_field(TransactTime::tag)->ival());
	EXPECT_EQ(*nos_new->find_field_value<string>(ExecInst::tag), nos_old.find_field(ExecInst::tag)->sval());
	EXPECT_EQ(*nos_new->find_field_value<char>(Side::tag), nos_old.find_field(Side::tag)->sval()[0]);
	EXPECT_EQ(*nos_new->find_field_value<double>(OrderQty::tag), nos_old.find_field(OrderQty::tag)->fval());
	EXPECT_EQ(*nos_new->find_field_value<double>(Price::tag), nos_old.find_field(Price::tag)->fval());
	EXPECT_EQ(nos_new->find_field(OrderQty::tag)->ival(), nos_old.find_field(OrderQty::tag)->ival()); // precision
	EXPECT_EQ(nos_new->find_field(Price::tag)->ival(), nos_old.find_field(Price::tag)->ival()); // precision

	const auto& gn1 = nos_new->find_group(NoAllocs::tag);
	const auto& go1 = nos_old.find_group(NoAllocs::tag);
	EXPECT_EQ(gn1->size(), go1->size());
	for(auto i = 0ul; i < gn1->size(); ++i)
	{
		const auto& fn1 = gn1->at(i);
		const auto& fo1 = go1->at(i);
		EXPECT_EQ(fn1->name(), fo1->get_name());
		EXPECT_EQ(fn1->long_name(), fo1->get_longname());
		EXPECT_EQ(fn1->seq(), fo1->get_seq());
		EXPECT_EQ(fn1->fields().size() + fn1->groups().size(), fo1->get_wmsg()->fields().size());
		EXPECT_EQ(*fn1->find_field_value<string>(AllocAccount::tag), fo1->find_field(AllocAccount::tag)->sval());
		EXPECT_EQ(*fn1->find_field_value<double>(AllocQty::tag), fo1->find_field(AllocQty::tag)->fval());
		EXPECT_EQ(fn1->find_field(AllocQty::tag)->ival(), fo1->find_field(AllocQty::tag)->ival()); // precision
		EXPECT_EQ(fn1->find_group(NoAllocs::tag), nullptr);
		EXPECT_EQ(fo1->find_group(NoAllocs::tag), nullptr);
		const auto& gn2 = fn1->find_group(NoPartyIDs::tag);
		const auto& go2 = fo1->find_group(NoPartyIDs::tag);
		EXPECT_EQ(gn2->size(), go2->size());
		for(auto j = 0ul; j < gn2->size(); ++j)
		{
			const auto& fn2 = gn2->at(j);
			const auto& fo2 = go2->at(j);
			EXPECT_EQ(fn2->fields().size() + fn2->groups().size(), fo2->get_wmsg()->fields().size());
			EXPECT_EQ(fn2->name(), fo2->get_name());
			EXPECT_EQ(fn2->long_name(), fo2->get_longname());
			EXPECT_EQ(fn2->seq(), fo2->get_seq());
			EXPECT_EQ(*fn2->find_field_value<string>(PartyID::tag), fo2->find_field(PartyID::tag)->sval());
		}
	}

	auto str_new = nos_new->wire_message()->SerializePartialAsString();
	auto str_old = nos_old.get_wmsg()->SerializePartialAsString();
	EXPECT_EQ(str_new, str_old);
	//cout << hex(str_new) << endl << hex(str_old) << endl;

	_uc.reset();
}

TEST(ufeedclient, test01)
{
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
			EXPECT_TRUE(false) << "ZeroMQ error happened: " << error;
			return false;
		}
		bool error_happened(const std::string &error, const std::exception &exception) override
		{
			EXPECT_TRUE(false) << "Error happened: " << error;
			return false;
		}
	} listener;

	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
		.max_io_threads(2)
		.poll_interval_ms(1)
		.subscriber(string(SUBSCRIBER_DEFAULT))
		.responder_topic("ufegw-requester")
		, &listener);

	_uc->start(false);

	// logon
	auto login = _uc->create_message();
	login.set_long_name("login")
		.set_type(WireMessage::st_system)
		.set_service_id(UFE_CMD_LOGIN)
		.add_field(UFE_CMD, Status{._status = UFE_CMD_LOGIN})
		.add_field(UFE_LOGIN_ID, "webuser")
		.add_field(UFE_LOGIN_PW, "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8");
	cerr << login << endl;
	auto response = _uc->request(move(login));
	EXPECT_EQ(1, listener._res_msgs.size());
	auto sess_token = listener._res_msgs[0]->find_field(UFE_SESSION_TOKEN);
	EXPECT_NE(sess_token, nullptr);
	auto opt_tkn{ listener._res_msgs[0]->find_field_value<Uuid>(UFE_SESSION_TOKEN) };
	EXPECT_TRUE(opt_tkn.has_value());
	EXPECT_FALSE(opt_tkn->_uuid.empty());

	// service list request
	auto svr_list_req = _uc->create_message();
	svr_list_req.set_long_name("service_list")
		.set_type(WireMessage::st_system)
		.set_service_id(UFE_CMD_SERVICE_LIST)
		.add_field(UFE_CMD, Status{._status = UFE_CMD_SERVICE_LIST});
	cerr << svr_list_req << endl;
	response = _uc->request(move(svr_list_req));
	cerr << *response << endl;
	EXPECT_GE(response->groups().size(), 1);
	auto services = response->find_group(UFE_SERVICE_RECORDS);
	EXPECT_GE(services->size(), 1);
	auto service = (*services)[0];
	auto service_name = service->find_field_value<std::string>(UFE_SERVICE_NAME);
	EXPECT_TRUE(service_name.has_value());
	EXPECT_FALSE(service_name->empty());
	auto target_compid = service->find_field(COMMON_TARGETCOMPID);
	auto tid = target_compid->sval();
	EXPECT_FALSE(tid.empty());

	// subscription check
	{
		unique_lock lk(listener._subs_lock);
		listener._subs_cond.wait_for(lk, 15s, [&listener]{ return !listener._sub_msgs.empty(); });
		EXPECT_FALSE(listener._sub_msgs.empty());
	}

	// authentication check
	// - this requires manual run of f8ptest initiator to connect to one of the auth session in ufegw
	{
		cerr << "waiting for auth..." << endl;
		unique_lock lk(listener._auth_lock);
		listener._auth_cond.wait_for(lk, 100s, [&listener]{ return listener._authenticated; });
		EXPECT_TRUE(listener._authenticated);
		cerr << "authenticated == " << listener._authenticated << endl;
	}

	// NOS creation
	using namespace FIX50SP2::Field;
	WireMessageGroup* grp{};
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
		.add_group(NoAllocs::tag, grp, [](UFEMessage::Builder& m, WireMessageGroup* grp)
			{
				m.add_group_item(grp)
					.set_long_name("NoAllocs")
					.set_type(WireMessage::st_fixmsg)
					.set_seq(1)
					.add_field(AllocAccount::tag, "ABC")
					.add_field(AllocQty::tag, 2);
				m.add_group_item(grp)
					.set_long_name("NoAllocs")
					.set_type(WireMessage::st_fixmsg)
					.set_seq(2)
					.add_field(AllocAccount::tag, "CDE")
					.add_field(AllocQty::tag, 4);
			})
		;
	cerr << nos << endl;
	_uc->request(move(nos));

	_uc->stop();
	_uc.reset();
}

#if 0
#include <ufecpp/ufe_cpp_fields_fix50sp2_tas.hpp>

TEST(ufeedclient, testJulius)
{
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
			EXPECT_TRUE(false) << "ZeroMQ error happened: " << error;
			return false;
		}
		bool error_happened(const std::string &error, const std::exception &exception) override
		{
			EXPECT_TRUE(false) << "Error happened: " << error;
			return false;
		}
	} listener;

	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
														.max_io_threads(2)
														.poll_interval_ms(1)
														.subscriber(SUBSCRIBER_DEFAULT)
														.responder_topic("ufegw-requester")
		, &listener);

	_uc->start(false);

	// logon
	auto login = _uc->create_message();
	login.set_long_name("login")
		.set_type(WireMessage::st_system)
		.set_service_id(UFE_CMD_LOGIN)
		.add_field(UFE_CMD, UFE_CMD_LOGIN)
		.add_field(UFE_LOGIN_ID, "admin")
		.add_field(UFE_LOGIN_PW, "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8");
	auto response = _uc->request(move(login));
	EXPECT_EQ(1, listener._res_msgs.size());
	EXPECT_NE(nullptr, listener._res_msgs[0]->find_field(UFE_SESSION_TOKEN));
	auto opt_tkn{ listener._res_msgs[0]->find_field_value<std::string>(UFE_SESSION_TOKEN) };
	EXPECT_TRUE(opt_tkn.has_value());
	EXPECT_FALSE(opt_tkn->empty());

	// service list request
	auto svr_list_req = _uc->create_message();
	svr_list_req.set_long_name("service_list")
		.set_type(WireMessage::st_system)
		.set_service_id(UFE_CMD_SERVICE_LIST)
		.add_field(UFE_CMD, UFE_CMD_SERVICE_LIST);
	response = _uc->request(move(svr_list_req));
	EXPECT_EQ(1, response->groups().size());

	// subscription check
	{
		unique_lock lk(listener._subs_lock);
		listener._subs_cond.wait_for(lk, 15s, [&listener]{ return !listener._sub_msgs.empty(); });
		EXPECT_FALSE(listener._sub_msgs.empty());
	}

	{
		using namespace FIX50SP2_TAS::Field;
		WireMessageGroup* grp{};
		struct Trade
		{
			int buy_order_id{1};
			string buy_party_id0{"bid0"s};
			string buy_party_id1{"bid1"s};
			string buy_party_id_source0{"bpids0"s};
			string buy_party_id_source1{"bpids1"s};
			string buy_party_role0{"bpr0"s};
			string buy_party_role1{"bpr1"s};

			int sell_order_id{2};
			string sell_party_id0{"spid0"s};
			string sell_party_id1{"spid1"s};
			string sell_party_id_source0{"spids0"s};
			string sell_party_id_source1{"spids1"s};
			string sell_party_role0{"spr0"s};
			string sell_party_role1{"spr1"s};
		};

		Trade tt;
		Trade* t = &tt;

		auto tae = _uc->create_message();
		tae.set_long_name("TradeCaptureReport")
			.set_type(WireMessage::st_fixmsg)
			.set_service_id(7)
			.set_subservice_id(0)
			.set_name(MsgType::TRADECAPTUREREPORT)

//    // 571 - TradeReportID
//    .add_field(TradeReportID::tag, t->trade_report_id)
			.add_field(TradeReportID::tag, "TEST001")

//    // 487 - TradeReportTransType
				//.add_field(TradeReportTransType::tag, t->trade_report_trans_type)
			.add_field(TradeReportTransType::tag, 0)

//    // 828 - TrdType
				//.add_field(TrdType::tag, t->trd_type)
			.add_field(TrdType::tag, 0)

//    // 17 - ExecID
				//if (t->exec_id) tae->add_field(ExecID::tag, t->exec_id);
				//.add_field(ExecID::tag, t->exec_id)
			.add_field(ExecID::tag, "0001")

//    // 55 - Symbol
				//.add_field(Symbol::tag, t->symbol)
			.add_field(Symbol::tag, "DEF")

//    // 32 - LastQty
				//.add_field(LastQty::tag, *(t->last_qty))
				// .add_field(LastQty::tag, 5)
			.add_field(LastQty::tag, 5.0)

//    // 31 - LastPx
				//.add_field(LastPx::tag, *(t->last_px))
				//.add_field(LastPx::tag, 1.1);
			.add_field(LastPx::tag, 1.1)

//    // 75 - TradeDate;
//    .add_field(TradeDate::tag, std::to_string(*(t->trade_date)))
			.add_field(TradeDate::tag, "20200205")

//    // 60 - TransactTime
//    .add_field(TransactTime::tag, t->transact_time)
			.add_field(TransactTime::tag, "20200205-10:00:00")

//    // 9001 - SerialTradeQualifier INV
//    tae->add_field(SerialTradeQualifier::tag, t->serial_trade_qualifier);
			.add_field(SerialTradeQualifier::tag, "N000")

//    // 9002 - TradeSerialNumber INV
//    tae->add_field(TradeSerialNumber::tag, t->trade_serial_no);
			.add_field(TradeSerialNumber::tag, 1)

//    // 552 - NoSides
				//.add_field(NoSides::tag, t->no_sides) //should be char?

//    auto* fld = tae->find_field(NoSides::tag);
//    auto& item0 = tae->add_message(fld, "NoSides", 0, WireMessage::st_fixmsg);
//    .add_field(NoSides::tag, "2")

			.add_group(NoSides::tag, grp, [&t](UFEMessage::Builder&m1, WireMessageGroup* g1)
			{
				WireMessageGroup* grp2{};
				m1.add_group_item(g1)
					.set_long_name("NoSides")
					.set_type(WireMessage::st_fixmsg)
					.set_seq(1)
						//.add_field(Side::tag, t->buy_side)
					.add_field(Side::tag, '1')
					.add_field(OrderID::tag, std::to_string(t->buy_order_id))
						//.add_field(Account::tag, t->buy_account)
					.add_field(Account::tag, "testacc0")
					.add_group(NoPartyIDs::tag, grp2, [&t](UFEMessage::Builder&m2, WireMessageGroup* g2)
					{
						m2.add_group_item(g2)
							.set_long_name("NoPartyIDs")
							.set_type(WireMessage::st_fixmsg)
							.set_seq(1)
							.add_field(PartyID::tag, t->buy_party_id0)
							.add_field(PartyIDSource::tag, t->buy_party_id_source0)
							.add_field(PartyRole::tag, t->buy_party_role0);

						m2.add_group_item(g2)
							.set_long_name("NoPartyIDs")
							.set_type(WireMessage::st_fixmsg)
							.set_seq(2)
							.add_field(PartyID::tag, t->buy_party_id1)
							.add_field(PartyIDSource::tag, t->buy_party_id_source1)
							.add_field(PartyRole::tag, t->buy_party_role1);
					})
					;
				m1.add_group_item(g1)
					.set_long_name("NoSides")
					.set_type(WireMessage::st_fixmsg)
					.set_seq(2)
						//.add_field(Side::tag, t->sell_side)
					.add_field(Side::tag, '2')
					.add_field(OrderID::tag, std::to_string(t->sell_order_id))
						//.add_field(Account::tag, t->sell_account)
					.add_field(Account::tag, "testacc1")
					.add_group(NoPartyIDs::tag, grp2, [&t](UFEMessage::Builder&m2, WireMessageGroup* g2)
					{
						m2.add_group_item(g2)
							.set_long_name("NoPartyIDs")
							.set_type(WireMessage::st_fixmsg)
							.set_seq(1)
							.add_field(PartyID::tag, t->sell_party_id0)
							.add_field(PartyIDSource::tag, t->sell_party_id_source0)
							.add_field(PartyRole::tag, t->sell_party_role0);

						m2.add_group_item(g2)
							.set_long_name("NoPartyIDs")
							.set_type(WireMessage::st_fixmsg)
							.set_seq(2)
							.add_field(PartyID::tag, t->sell_party_id1)
							.add_field(PartyIDSource::tag, t->sell_party_id_source1)
							.add_field(PartyRole::tag, t->sell_party_role1);
					})
					;
			});
		auto rep = _uc->request(move(tae));
		EXPECT_EQ(rep->find_field_value<Status>(UFE_RESPONSE_CODE)->_status, UFE_OK);
	}

	_uc->stop();
	_uc.reset();
}

#endif
