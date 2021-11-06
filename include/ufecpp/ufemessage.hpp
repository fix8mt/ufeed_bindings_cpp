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

#ifndef UFECPPBINDING_UFEMESSAGE_HPP
#define UFECPPBINDING_UFEMESSAGE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>

#include <ufecpp/ufecppdll.h>
#include <ufecpp/ufecppversion.h>
#include <ufecpp/ufeapi.pb.h>

namespace UFECPP
{
	using namespace UFE;
	namespace gpb = google::protobuf;

	/// Status field type
	struct Status
	{
		int64_t _status;
	};
	/// UUID field type
	struct Uuid
	{
		std::string _uuid;
	};
	/// timpoint field type
	using TimePoint = std::chrono::system_clock::time_point;

	/**
	 * UFE base message builder class
	 * \example
	 * 	auto _uc = make_shared<UFEedClient>(UFEedConfiguration()...);
	 * 	using namespace FIX50SP2::Field;
	 * 	WireMessageGroup* grp{};
	 * 	auto nos_bld = _uc->create_message();
	 * 	nos.set_long_name("NewOrderSingle")
	 * 		.set_type(WireMessage::st_fixmsg)
	 * 		.set_service_id(1)
	 * 		.set_name(MsgType::NEWORDERSINGLE)
	 * 		.add_field(ClOrdID::tag, "123")
	 * 		.add_field(TransactTime::tag, std::chrono::system_clock::now())
	 * 		.add_field(ExecInst::tag, ExecInst::ALL_OR_NONE)
	 * 		.add_field(OrdType::tag, OrdType::LIMIT)
	 * 		.add_field(Side::tag, Side::BUY)
	 * 		.add_group(NoAllocs::tag, grp, [](UFEMessage& m, WireMessageGroup* wmg, GroupsContainer& grp)
	 * 			{
	 * 		 	 	m.add_group_item(wmg, grp)
	 * 		 				->set_long_name("NoAlloc")
	 * 		 	 		.set_type(WireMessage::st_fixmsg)
	 * 		 	 		.set_seq(1)
	 * 		 	 		.add_field(AllocAccount::tag, "ABC")
	 * 		 	 		.add_field(AllocQty::tag, 2);
	 * 		 	 	m.add_group_item(wmg, grp)
	 * 		 	 		->set_long_name("NoAlloc")
	 * 		 	 		.set_type(WireMessage::st_fixmsg)
	 * 		 	 		.set_seq(2)
	 * 		 	 		.add_field(AllocAccount::tag, "CDE")
	 * 		 	 		.add_field(AllocQty::tag, 4);
	 * 			})
	 * 		;
	 * 	auto nos = nos_bld.build();
	 */
	class UFEMessage
	{
	public:
		/// UFE message unique ptr
		using UFEMessagePtr = std::shared_ptr<UFEMessage>;
		using ArenaPtr = std::shared_ptr<gpb::Arena>;

		struct WireMessageWithArena
		{
			ArenaPtr _arena;
			WireMessage *_wm{};
		};

		class Builder
		{
			friend class UFEMessage;

		public:
			/// Return null wire message
			static const WireMessage* null_wiremessage() { return nullptr; }
			/// Wire message group
			using WireMessageGroup = gpb::RepeatedPtrField<WireMessage>;

			/**
			 * Ctor
			 */
			explicit Builder() 
			{
				_wmwa._arena = std::make_shared<gpb::Arena>();
				_wmwa._wm = gpb::Arena::CreateMessage<WireMessage>(_wmwa._arena.get());
			}
			/**
			 * Ctor
			 * \param builder Builder to take ownership from
			 */
			Builder(Builder&& builder) noexcept: _wmwa(std::move(builder._wmwa)) { }
			/**
			 * Ctor
			 * \param wmwa WireMessageWithArena to take ownership from
			 */
			Builder(WireMessageWithArena&& wmwa) noexcept: _wmwa(std::move(wmwa)) { }

			/// No copy ctor - use UFEMessage::clone()
			Builder(const Builder&) = delete;
			/// No operator= - use UFEMessage::clone()
			Builder& operator=(const Builder&) = delete;

			/// Returns service id
			int service_id() const { return _wmwa._wm->service_id(); }
			/// Sets service id, \return *this
			Builder& set_service_id(int service_id) { _wmwa._wm->set_service_id(service_id); return *this; }
			/// Return sub service id
			int subservice_id() const { return _wmwa._wm->subservice_id(); }
			/// Sets subservice id, \return *this
			Builder& set_subservice_id(int subservice_id) { _wmwa._wm->set_subservice_id(subservice_id); return *this; }
			/// Returns seq
			uint32_t seq() const { return _wmwa._wm->seq(); }
			/// Sets seq, \return *this
			Builder& set_seq(uint32_t seq) { _wmwa._wm->set_seq(seq); return *this; }
			/// Returns wire message type
			WireMessage::Type type() const { return _wmwa._wm->type(); }
			/// Sets wire message type, \return *this
			Builder& set_type(WireMessage::Type type) { _wmwa._wm->set_type(type); return *this; }
			/// Returns message long name
			const std::string& long_name() const { return _wmwa._wm->longname(); }
			/// Sets message long name
			Builder& set_long_name(std::string long_name) { _wmwa._wm->set_longname(std::move(long_name)); return *this; }
			/// Return message name
			const std::string& name() const { return _wmwa._wm->name(); }
			/// Set message name, \return *this
			Builder& set_name(std::string name) { _wmwa._wm->set_name(std::move(name)); return *this; }
			/// Return inner WireMessage pointer
			const WireMessage* wire_message() const { return _wmwa._wm; }

			/**
			 * Adds integer field
			 * \param tag integer field tag
			 * \param val integer value
			 * \param loc integer location
			 * \return *this
			 */
			template<typename T, std::enable_if_t<
				std::is_same<T,  int64_t>::value || std::is_same<T,  int32_t>::value || std::is_same<T,  int16_t>::value ||
				std::is_same<T, uint64_t>::value || std::is_same<T, uint32_t>::value || std::is_same<T, uint16_t>::value ||
				std::is_enum<T>::value
				> * = nullptr>
			Builder& add_field(uint32_t tag, T val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_int, loc)->set_ival(static_cast<int64_t>(val)); return *this; }
			/**
			 * Adds char field
			 * \param tag integer field tag
			 * \param val integer value
			 * \param loc integer location
			 * \return *this
			 */
			template<typename T, std::enable_if_t<
				std::is_same<T,  char>::value || std::is_same<T, unsigned char>::value
				> * = nullptr>
			Builder& add_field(uint32_t tag, T val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_char, loc)->set_sval(&val, 1); return *this; }
			/**
			 * Adds double field
			 * \param tag double field tag
			 * \param val double val
			 * \param precision double decimal points
			 * \param loc double location
			 * \return *this
			 */
			template<typename T, std::enable_if_t<
				std::is_same<T,  double>::value || std::is_same<T, float>::value
				> * = nullptr>
			Builder& add_field(uint32_t tag, T val, int precision = 2, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ auto * fld = add_field_common(tag, UFEField::ft_double, loc); fld->set_fval(static_cast<double>(val)); fld->set_ival(precision); return *this; }
			/**
			 * Adds string field
			 * \param tag string field tag
			 * \param val string value
			 * \param loc string location
			 * \return *this
			 */
			Builder& add_field(uint32_t tag, const char* val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_string, loc)->set_sval(val); return *this; }
			/**
			 * Adds string field
			 * \param tag string field tag
			 * \param val string value
			 * \param loc string location
			 * \return *this
			 */
			template<typename T, std::enable_if_t<
				std::is_same<typename std::decay<T>::type, std::string>::value ||
				std::is_same<typename std::decay<T>::type, std::string_view>::value
				> * = nullptr>
			Builder& add_field(uint32_t tag, T&& val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_string, loc)->set_sval(std::forward<T>(val)); return *this; }
			/**
			 * Adds bool field
			 * \param tag bool field tag
			 * \param val bool value
			 * \param loc bool location
			 * \return *this
			 */
			Builder& add_field(uint32_t tag, bool val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_bool, loc)->set_bval(val); return *this; }
			/**
			 * Adds time field
			 * \param tag time field tag
			 * \param val time value
			 * \param loc time location
			 * \return *this
			 */
			Builder& add_field(uint32_t tag, const TimePoint& val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_time, loc)->set_ival(std::chrono::duration_cast<std::chrono::nanoseconds>(val.time_since_epoch()).count()); return *this; }
			/**
			 * Adds UUID field
			 * \param tag uuid tag
			 * \param val uuid string representation
			 * \param loc uuid location
			 * \return *this
			 */
			Builder& add_field(uint32_t tag, const Uuid& val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_uuid, loc)->set_sval(val._uuid); return *this; }
			/**
			 * Adds status field
			 * \param tag status tag
			 * \param val status value
			 * \param loc status location
			 * \return *this
			 */
			Builder& add_field(uint32_t tag, Status val, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{ add_field_common(tag, UFEField::ft_status, loc)->set_ival(val._status); return *this; }
			/**
			 * Added a set of fields to message
			 * \tparam Iter iterator type
			 * \param first begin iterator
			 * \param last end iterator
			 * \return *this
			 */
			template<typename Iter>
			Builder& add_fields(Iter first, Iter last)
			{ for(; first != last; ++first) _wmwa._wm->mutable_fields()->Add(*first); return *this; }
			/**
			 * Adds group field to the message
			 * \param to field to add to
			 * \param tr group creation lambda
			 * \param loc status location
			 * \return created message unique_ptr ref
			 */
			Builder& add_group(uint32_t tag, WireMessageGroup* &grp, const std::function<void(Builder&, WireMessageGroup*)>& tr={}, UFEField::UFEFieldLocation loc = UFEField::fl_body)
			{
				auto *mval{add_field_common(tag, UFEField::ft_msg, loc)};
				grp = mval->mutable_mval();
				if (tr) tr(*this, grp);
				mval->set_ival(grp->size()); // @todo: until UFE-145
				return *this;
			}
			/*! Add group item
			 * \param grp wire message group pointer
			 * \param grp_cont ufe message group container
			 * \param location field location
			 * \return *this
			 */
			Builder add_group_item(WireMessageGroup* grp, UFEField::UFEFieldLocation location = UFEField::fl_body)
			{ auto *wm = grp->Add(); wm->set_type(WireMessage::st_fixmsg); return Builder(wm); }

			/*! Builds a message from message builder
			 * \return created UFEMessage ptr
			 */
			UFEMessagePtr build()
			{ return UFEMessagePtr{new UFEMessage(std::move(_wmwa))}; } // make_shared doesnt work since ctor is private

			/*! Prints a message content to the output stream
			 * \param os ostream to print message content to
			 * \return ostream with printed message content
			 */
			std::ostream& print(std::ostream& os) const
			{ return print_wm(os, *_wmwa._wm); }

			friend std::ostream& operator<<(std::ostream& os, const Builder& what)
			{ return what.print(os); }

		private:
			WireMessageWithArena _wmwa{};

			explicit Builder(WireMessage * wm)
			{
				_wmwa._wm = wm;
				// _wmwa._arena must be nullptr here since it is set at upper level
			}

			UFEField* add_field_common(uint32_t tag, UFEField::UFEFieldType type, UFEField::UFEFieldLocation loc = UFEField::fl_body) const
			{
				auto * field = _wmwa._wm->add_fields();
				field->set_tag(tag);
				field->set_type(type);
				field->set_location(loc);
				return field;
			}

			UFECPPAPI static std::ostream& print_wm(std::ostream& os, const WireMessage& wm, int depth = 0);
		};

		friend class UFEMessage::Builder;

		/// UFE message fields map
		using FieldsMap = std::unordered_map<uint32_t, UFEField*>;
		/// UFE message groups container
		using GroupsContainer = std::vector<UFEMessagePtr>;
		/// UFE message groups map
		using GroupsMap = std::unordered_map<uint32_t, GroupsContainer>;

		/// No copy ctor - use clone()
		UFEMessage(const UFEMessage&) = delete;
		/// No operator= - use clone()
		UFEMessage& operator=(const UFEMessage&) = delete;
		/**
		 * Clone UFE message
		 * \return clonned message unique ptr
		 */
		UFEMessagePtr clone() const
		{
			WireMessageWithArena wmwa;
			wmwa._arena = std::make_unique<gpb::Arena>();
			wmwa._wm = gpb::Arena::CreateMessage<WireMessage>(_wmwa._arena.get());
			wmwa._wm->CopyFrom(*_wmwa._wm);
			return UFEMessagePtr{new UFEMessage(std::move(wmwa))}; // make_shared doesnt work since ctor is private
		}

		/// Returns service id
		int service_id() const { return _wmwa._wm->service_id(); }
		/// Sets service id, \return *this
		UFEMessage& set_service_id(int service_id) { _wmwa._wm->set_service_id(service_id); return *this; }
		/// Return sub service id
		int subservice_id() const { return _wmwa._wm->subservice_id(); }
		/// Sets subservice id, \return *this
		UFEMessage& set_subservice_id(int subservice_id) { _wmwa._wm->set_subservice_id(subservice_id); return *this; }
		/// Returns seq
		uint32_t seq() const { return _wmwa._wm->seq(); }
		/// Sets seq, \return *this
		UFEMessage& set_seq(uint32_t seq) { _wmwa._wm->set_seq(seq); return *this; }
		/// Returns wire message type
		WireMessage::Type type() const { return _wmwa._wm->type(); }
		/// Sets wire message type, \return *this
		UFEMessage& set_type(WireMessage::Type type) { _wmwa._wm->set_type(type); return *this; }
		/// Returns message long name
		const std::string& long_name() const { return _wmwa._wm->longname(); }
		/// Sets message long name
		UFEMessage& set_long_name(std::string long_name) { _wmwa._wm->set_longname(std::move(long_name)); return *this; }
		/// Return message name
		const std::string& name() const { return _wmwa._wm->name(); }
		/// Set message name, \return *this
		UFEMessage& set_name(std::string name) { _wmwa._wm->set_name(std::move(name)); return *this; }

		/// Gets mapped fields, \return field map
		const FieldsMap& fields() const { return _fields; }
		/// Gets mapped groups, \return group map
		const GroupsMap& groups() const { return _groups; }
		/// Gets wire mesage, \return wire message
		const WireMessage* wire_message() const { return _wmwa._wm; }

		/**
		 * Finds field
		 * \param tag field tag to find
		 * \return UFEField pointer if found, otherwise nullptr
		 */
		const UFEField* find_field(uint32_t tag) const
		{
			auto it = _fields.find(tag);
			return it != _fields.end() ? it->second : nullptr;
		}
		/**
		 * Finds field
		 * \param tag field tag to find
		 * \return UFEField pointer if found, otherwise nullptr
		 */
		UFEField* find_field(uint32_t tag)
		{
			auto it = _fields.find(tag);
			return it != _fields.end() ? it->second : nullptr;
		}
		/**
		 * Finds group
		 * \param tag group tag to find
		 * \return GroupsContainer pointer if found, otherwise nullptr
		 */
		const GroupsContainer* find_group(uint32_t tag) const
		{
			auto it = _groups.find(tag);
			return it != _groups.end() ? &it->second : nullptr;
		}

		/*! Finds field value
		 * \tparam T type of the field value to find
		 * \param tag tag og the filed to find
		 * \return optional of field value
		 */
		template<typename T>
		std::optional<T> find_field_value(uint32_t tag) const
		{
			const auto * field = find_field(tag);
			return field ? std::optional<T>(extract_field_value<T>(*field)) : std::nullopt;
		}

		/*! Creates a new builder for the message
		 * \return new builder for the message
		 */
		UFEMessage::Builder new_builder()
		{
			return UFEMessage::Builder(std::move(_wmwa));
		}

		/*! Prints a message content to the output stream
		 * \param os ostream to print message content to
		 * \return ostream with printed message content
		 */
		std::ostream& print(std::ostream& os) const
		{ return Builder::print_wm(os, *_wmwa._wm); }

		friend std::ostream& operator<<(std::ostream& os, const UFEMessage& what)
		{ return what.print(os); }

	private:
		WireMessageWithArena _wmwa;
		FieldsMap _fields;
		GroupsMap _groups;

		explicit UFEMessage(WireMessageWithArena&& wmwa)
			: _wmwa(std::move(wmwa))
		{
			if(_wmwa._wm)
				remap_wire_message();
		}

		void remap_field(UFEField* field)
		{
			if (field->type() == UFEField::ft_msg)
			{
				auto gitr = _groups.find(field->tag());
				if (gitr == _groups.end())
					gitr = _groups.insert(std::make_pair(field->tag(), GroupsContainer())).first;
				for(auto& mval: *field->mutable_mval())
					gitr->second.emplace_back(new UFEMessage(WireMessageWithArena{nullptr, &mval})); // arena must be nullptr here since it is set at upper level
			}
			else
				_fields[field->tag()] = field;
		}

		void remap_wire_message()
		{
			for(auto& field: *_wmwa._wm->mutable_fields())
				remap_field(&field);
		}

		template<typename T> T extract_field_value(const UFEField& field) const;
	};

	template<>
	inline double UFEMessage::extract_field_value<double>(const UFEField &field) const { return field.fval(); }
	template<>
	inline int64_t UFEMessage::extract_field_value<int64_t>(const UFEField &field) const { return field.ival(); }
	template<>
	inline std::string UFEMessage::extract_field_value<std::string>(const UFEField& field) const { return field.sval(); }
	template<>
	inline bool UFEMessage::extract_field_value<bool>(const UFEField& field) const { return field.bval(); }
	template<>
	inline Status UFEMessage::extract_field_value<Status>(const UFEField& field) const { return Status{ ._status = field.ival() }; }
	template<>
	inline Uuid UFEMessage::extract_field_value<Uuid>(const UFEField& field) const { return Uuid{ ._uuid = field.sval() }; }
	template<>
	inline TimePoint UFEMessage::extract_field_value<TimePoint>(const UFEField& field) const { return TimePoint(std::chrono::nanoseconds(field.ival())); }
	template<>
	inline char UFEMessage::extract_field_value<char>(const UFEField& field) const { return field.sval()[0]; }

	using UFEMessagePtr = UFEMessage::UFEMessagePtr;
	using WireMessageGroup = UFEMessage::Builder::WireMessageGroup;
	using GroupsContainer = UFEMessage::GroupsContainer;
	using WireMessageWithArena = UFEMessage::WireMessageWithArena;
}

#endif //UFECPPBINDING_UFEMESSAGE_HPP
