//-------------------------------------------------------------------------------------------------
/*
$Id: 073a7ab64e1500830258895b3f4a44beb79ef5cd $
    ____                      __      ____
   /\  _`\   __             /'_ `\   /\  _`\
   \ \ \L\_\/\_\    __  _  /\ \L\ \  \ \ \L\ \ _ __    ___
    \ \  _\/\/\ \  /\ \/'\ \/_> _ <_  \ \ ,__//\`'__\ / __`\
     \ \ \/  \ \ \ \/>  </   /\ \L\ \  \ \ \/ \ \ \/ /\ \L\ \
      \ \_\   \ \_\ /\_/\_\  \ \____/   \ \_\  \ \_\ \ \____/
       \/_/    \/_/ \//\/_/   \/___/     \/_/   \/_/  \/___/

               Fix8Pro FIX Engine and Framework

Copyright (C) 2010-20 Fix8 Market Technologies Pty Ltd (ABN 29 167 027 198)
All Rights Reserved. [http://www.fix8mt.com] <heretohelp@fix8mt.com>

THIS FILE IS PROPRIETARY AND  CONFIDENTIAL. NO PART OF THIS FILE MAY BE REPRODUCED,  STORED
IN A RETRIEVAL SYSTEM,  OR TRANSMITTED, IN ANY FORM OR ANY MEANS,  ELECTRONIC, PHOTOSTATIC,
RECORDED OR OTHERWISE, WITHOUT THE PRIOR AND  EXPRESS WRITTEN  PERMISSION  OF  FIX8  MARKET
TECHNOLOGIES PTY LTD.

*/
//-------------------------------------------------------------------------------------------------
#ifndef UFECPPBINDING_DEPRECATED_UFEAPI_HPP_
#define UFECPPBINDING_DEPRECATED_UFEAPI_HPP_

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#include <chrono>
#include <functional>

#ifdef _MSC_VER
# include <Rpc.h> // for UUID
using uuid = UUID;
#else
# include <uuid/uuid.h>
struct uuid { uuid_t _id; };
#endif

#include <ufecpp/ufecppversion.h>
#include <ufecpp/ufecppdll.h>
#include <ufecpp/ufeapi.pb.h>

//-------------------------------------------------------------------------------------------------
namespace UFECPP_DEPRECATED {

using namespace UFE;

//-------------------------------------------------------------------------------------------------
constexpr unsigned non_fix_field(std::numeric_limits<unsigned short>::max());
constexpr bool is_nff(unsigned what) { return what > non_fix_field; }

//-------------------------------------------------------------------------------------------------
using UFEFields = std::unordered_map<unsigned, const UFEField *>;
using UFEFieldIdxs = std::unordered_map<unsigned, unsigned>;
using UFEGroup = std::vector<const class UFEMessage *>;
using UFEGroups = std::unordered_map<unsigned, UFEGroup>;
using UFEMessagePtr = std::shared_ptr<UFEMessage>;
using TranslateFunc = std::function<const char *(int64_t)>;
using TimePoint = std::chrono::system_clock::time_point;

//-------------------------------------------------------------------------------------------------
class UFEMessage
{
	std::string _name, _longname;
	uint32_t _seq{};
	int32_t _serviceid, _subserviceid, _type;
	const WireMessage *_wmsg{};
	const bool _own{};
	UFEFields _fields, _nf_fields;
	UFEFieldIdxs _fieldidxs;
	UFEGroups _groups;

	unsigned remap(const WireMessage& msg)
	{
		unsigned idx{};
		for (const auto& pp : msg.fields())
		{
			is_nff(pp.tag()) ? _nf_fields.emplace(pp.tag(), &pp) : _fields.emplace(pp.tag(), &pp);
			_fieldidxs.emplace(pp.tag(), idx++);
			if (pp.type() == UFEField::ft_msg)
			{
				for (const auto& qq : pp.mval())
				{
					auto *nm(new UFEMessage(qq));
					auto gitr(_groups.find(pp.tag()));
					if (gitr == _groups.end())
						gitr = _groups.emplace(pp.tag(), UFEGroup()).first;
					gitr->second.emplace_back(nm);
				}
			}
		}
		return static_cast<unsigned>(_fields.size());
	}

	void del_groups()
	{
		for (const auto& pp : _groups)
			for (const auto *qq : pp.second)
				delete qq;
	}

public:
	UFEMessage() {}
	virtual ~UFEMessage()
	{
		if (_own)
			delete _wmsg;
		del_groups();
	}

	UFEMessage(const WireMessage& msg, bool own=false) : _name(msg.name()), _longname(msg.longname()),
	  _seq(msg.seq()), _serviceid(msg.service_id()), _subserviceid(msg.subservice_id()),_type(msg.type()), _wmsg(&msg), _own(own)
	{
		remap(msg);
	}
	UFEMessage(const WireMessage *msg, bool own=false) : _name(msg->name()), _longname(msg->longname()),
	  _seq(msg->seq()), _serviceid(msg->service_id()), _subserviceid(msg->subservice_id()),_type(msg->type()), _wmsg(msg), _own(own)
	{
		remap(*msg);
	}
	UFEMessage& operator=(const WireMessage& msg)
	{
		_name = msg.name();
		_longname = msg.longname();
		_seq = msg.seq();
		_serviceid = msg.service_id();
		_subserviceid = msg.subservice_id();
		_type = msg.type();
		_wmsg = &msg;
		del_groups();
		_fields.clear();
		_nf_fields.clear();
		_fieldidxs.clear();
		_groups.clear();
		remap(msg);
		return *this;
	}

	static UFEMessagePtr factory(const std::string& msg)
	{
		auto *wm(new WireMessage);
		wm->ParseFromString(msg);
		UFEMessagePtr ufe(new UFEMessage(*wm, true));
		ufe->_wmsg = wm;
		return ufe;
	}

	const UFEField *find_field(unsigned tag) const
	{
		const auto result(is_nff(tag) ? _nf_fields.find(tag) : _fields.find(tag));
		return result != _fields.cend() ? result->second : nullptr;
	}

	UFEField *find_field(unsigned tag)
	{
		auto result(is_nff(tag) ? _nf_fields.find(tag) : _fields.find(tag));
		return result != _fields.end() ? const_cast<UFEField *>(result->second) : nullptr;
	}

	unsigned find_field_idx(unsigned tag) const
	{
		const auto result(_fieldidxs.find(tag));
		return result != _fieldidxs.cend() ? result->second : non_fix_field;
	}

	unsigned group_size(unsigned tag) const
	{
		const auto result(_groups.find(tag));
		return static_cast<unsigned>(result != _groups.cend() ? result->second.size() : 0);
	}

	const UFEGroup *find_group(unsigned tag) const
	{
		const auto result(_groups.find(tag));
		return result != _groups.cend() ? &result->second : nullptr;
	}

	virtual void print(std::vector<std::ostream *> os, int depth=0, TranslateFunc trf=TranslateFunc()) const
	{
		for (auto ii : os)
			print(*ii, depth, trf);
	}

	static std::ostream& print_uuid(std::ostream& os, const std::string& uuid)
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

	virtual void print(std::ostream& os, int depth=0, TranslateFunc trf=TranslateFunc()) const
	{
		const std::string dspacer((1 + depth) * 3, ' ');
		os << std::string(depth * 3, ' ') << "srvc_id=" << _serviceid << " subsrvc_id=" << _subserviceid << " type=" << _type;
		if	(!_name.empty())
			os << " msg=" << _name;
		if (!_longname.empty())
			os << " (" << _longname << ')';
		os << " seq=" << _seq << std::endl;

		auto pfields([&](const UFEFields& fields)->void
		{
			for (const auto& pp : fields)
			{
				os << dspacer;
				if (const char *result { trf ? trf(pp.first) : nullptr }; result)
					os << result << '(' << pp.first << ')';
				else
					os << pp.first;
				os << " (";
				switch(pp.second->location())
				{
				case UFEField::fl_header: os << "hdr"; break;
				case UFEField::fl_body: os << "body"; break;
				case UFEField::fl_trailer: os << "trl"; break;
				case UFEField::fl_system: os << "sys"; break;
				default: os << "unknown"; break;
				}
				os << "): ";
				switch(pp.second->type())
				{
				case UFEField::ft_status:
					if (const char *sresult { trf ? trf(pp.second->ival()) : nullptr }; sresult)
					{
						os << sresult;
						if (trf)
							os << '(' << pp.second->ival() << ')';
						os << std::endl;
						break;
					}
					[[fallthrough]];
				case UFEField::ft_int: os << dspacer << pp.second->ival() << std::endl; break;
				case UFEField::ft_bool: os << dspacer << (pp.second->bval() ? 'Y' : 'N') << std::endl; break;
				case UFEField::ft_char: os << dspacer << pp.second->sval()[0] << std::endl; break;
				case UFEField::ft_string: os << dspacer << pp.second->sval() << std::endl; break;
				case UFEField::ft_uuid: os << dspacer; print_uuid(os, pp.second->sval()); os << std::endl; break;
				case UFEField::ft_double: os << dspacer << pp.second->fval() << std::endl; break;
				case UFEField::ft_msg:
					os << dspacer << pp.second->ival() << " elements, depth=" << depth << " ... ";
					if (const auto *grp(find_group(pp.second->tag())); grp)
					{
						os << std::endl;
						for (const auto *qq : *grp)
							qq->print(os, depth + 1, trf);
					}
					break;
				case UFEField::ft_time:
					{
#if defined(__APPLE__) || defined(_MSC_VER)
						using this_time_point = std::chrono::time_point<std::chrono::system_clock>;
#else
						using this_time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;
#endif
						const this_time_point val(std::chrono::duration_cast<this_time_point::duration>(std::chrono::nanoseconds(pp.second->ival())));
						const std::time_t valc(std::chrono::system_clock::to_time_t(val));
						os << std::put_time(std::gmtime(&valc), "%F %T")
							<< '.' << std::chrono::duration_cast<std::chrono::nanoseconds>(val.time_since_epoch()).count() % std::nano::den << std::endl;
					}
					break;
				default: os << "Unknown field type:" << pp.second->type() << std::endl; break;
				}
			}
		});

		pfields(_fields);
		pfields(_nf_fields);
	}

	const std::string& get_name() const { return _name; }
	const std::string& get_longname() const { return _longname; }
	uint32_t get_seq() const { return _seq; }
	uint32_t get_type() const { return _type; }
	int32_t get_serviceid() const { return _serviceid; }
	int32_t get_subserviceid() const { return _subserviceid; }
	const WireMessage *get_wmsg() const { return _wmsg; }

	friend std::ostream& operator<<(std::ostream& os, const UFEMessage& what)
	{
		what.print(os);
		return os;
	}
};

//-------------------------------------------------------------------------------------------------
inline UFEField *add_field(WireMessage *to, int32_t tag, UFEField_UFEFieldType tp, UFEField_UFEFieldLocation loc=UFEField::fl_system)
{
	auto *fld(to->add_fields());
	fld->set_tag(tag);
	fld->set_location(loc);
	fld->set_type(tp);
	return fld;
}

inline bool remove_field(const UFEMessage *umsg, WireMessage *from, int32_t tag)
{
	if (auto result { umsg->find_field_idx(tag) }; result != non_fix_field)
	{
		from->mutable_fields()->erase(from->mutable_fields()->begin() + result);
		return true;
	}
	return false;
}

inline WireMessage *add_msg(UFEField *to, const char *name, uint32_t seq, WireMessage_Type tp)
{
	auto *mval(to->add_mval()); // WireMessage
	mval->set_name(name);
	mval->set_seq(seq);
	mval->set_type(tp);
	return mval;
}

inline UFEField *add_msg_field(UFEField *to, int32_t tag)
{
	return add_field(to->add_mval(), tag, UFEField::ft_msg);
}

inline WireMessage *add_msg(UFEField *to, const std::string& name, uint32_t seq, WireMessage_Type tp)
{
	return add_msg(to, name.c_str(), seq, tp);
}

//-------------------------------------------------------------------------------------------------
inline UFEField *add_status(WireMessage *to, int32_t tag, int64_t value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_status, loc) };
	fld->set_ival(value);
	return fld;
}

inline UFEField *add_int(WireMessage *to, int32_t tag, int64_t value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_int, loc) };
	fld->set_ival(value);
	return fld;
}

inline UFEField *add_time(WireMessage *to, int32_t tag, int64_t value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_time, loc) };
	fld->set_ival(value);
	return fld;
}

inline UFEField *add_time(WireMessage *to, int32_t tag, const TimePoint& value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_time, loc) };
	fld->set_ival(std::chrono::duration_cast<std::chrono::nanoseconds>(value.time_since_epoch()).count());
	return fld;
}

inline UFEField *add_string(WireMessage *to, int32_t tag, const char *value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_string, loc) };
	fld->set_sval(value);
	return fld;
}

inline UFEField *add_string(WireMessage *to, int32_t tag, const std::string& value, UFEField_UFEFieldLocation loc)
{
	return add_string(to, tag, value.c_str(), loc);
}

inline UFEField *add_string(WireMessage *to, int32_t tag, std::string_view value, UFEField_UFEFieldLocation loc)
{
	return add_string(to, tag, value.data(), loc);
}

inline UFEField *add_uuid(WireMessage *to, int32_t tag, const std::string& value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_uuid, loc) };
	fld->set_sval(value);
	return fld;
}

inline UFEField *add_uuid(WireMessage *to, int32_t tag, const void *value, size_t sz, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_uuid, loc) };
	fld->set_sval(value, sz);
	return fld;
}

inline UFEField *add_char(WireMessage *to, int32_t tag, char value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_char, loc) };
	fld->set_sval(&value, 1);
	return fld;
}

inline UFEField *add_bool(WireMessage *to, int32_t tag, bool value, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_bool, loc) };
	fld->set_bval(value);
	return fld;
}

inline UFEField *add_double(WireMessage *to, int32_t tag, double value, int64_t precision, UFEField_UFEFieldLocation loc)
{
	auto *fld { add_field(to, tag, UFEField::ft_double, loc) };
	fld->set_fval(value);
	fld->set_ival(precision);
	return fld;
}

template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_status(Args&& ...args) { return add_status(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_bool(Args&& ...args) { return add_bool(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_double(Args&& ...args) { return add_double(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_string(Args&& ...args) { return add_string(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_int(Args&& ...args) { return add_int(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_char(Args&& ...args) { return add_char(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_uuid(Args&& ...args) { return add_uuid(std::forward<Args>(args)..., loc); }
template<UFEField_UFEFieldLocation loc, typename... Args>
UFEField *add_time(Args&& ...args) { return add_time(std::forward<Args>(args)..., loc); }

//-------------------------------------------------------------------------------------------------
} // UFECPP_DEPRECATED

#endif // UFECPPBINDING_DEPRECATED_UFEAPI_HPP_
