<p align="center">
  <a href="https://www.fix8mt.com"><img src="fix8mt_Master_Logo_Green_Trans.png" width="200"></a>
</p>

# UFEed C++ Binding

-   [Introduction](#introduction)
-   [Getting started](#getting-started)
-   [Interface](#interface)
    -   [UFEMessage Builder](#ufemessage-builder)
    -   [UFEMessage](#ufemessage)
    -   [UFEedClient](#ufeedclient)
-   [Constants](#constants)
    -   [FIX variants constants](#fix-variants-constants)
-   [Building](#building)


------------------------------------------------------------------------

# Introduction

The UFEed C++ Binding (`UFEed_C++`) provides a low level C++ interface to the
UFEGW. Interactions with the UFEGW are based around a `UFEedClient`
object from `UFECPP` namespace which can be used to send and receive
Messages to and from the UFEGW.

Use the following [Universal FIX engine documentaion](https://fix8mt.atlassian.net/wiki/spaces/FMT/pages/634438/Universal+FIX+Engine+Home) for a reference.

Features of `UFEedClient`:

-   System API support ([see 4. Implementation Guide - Section
    1.3](https://fix8mt.atlassian.net/wiki/spaces/FIX8PRO/pages/628308/4.+Implementation+Guide))

-   Business API support (eg. NewOrderSingle and standard FIX messages)

-   Provides a 4-way communications API in order to make requests,
    publish messages, receive responses and subscribe to broadcast
    messages

-   User defined implementation of callback interface to handle these
    PUB, SUB, REQ and REP message events

-   Dynamic configuration of PUB, SUB, REQ, REP addressing and topics

-   Internal session management

Features of a `UFEMessage/UFEMessage::Builder`:

-   A generic container of fields, i.e. tag/typed value pairs with
    possible nested messages called groups

-   Smart field creation and access, rendering field value to ival, sval
    or fval depending on context

-   Named Message properties (name, long_name, seq, service_id,
    subservice_id)

# Getting started

The `UFEed_C++` is provided as a shared library in either .so (Linux) or .dll
(Windows) format. It has no external dependencies besides Google
Protobuf library header files that are included in the installation
package. Google Protobuf and ZeroMQ libraries are linked in to `UFEed_C++`
statically. Installation directory structure after the build (example is Linux) is as follows:

```
ufeed_bindings_cpp
├── include
│   ├── 3rdparty
│   │   └── google
│   │       └── protobuf
│   └── ufecpp
│       ├── ufe_cpp_fields_fix40.hpp
│       ├── ufe_cpp_fields_fix41.hpp
│       ├── ufe_cpp_fields_fix42.hpp
│       ├── ufe_cpp_fields_fix43.hpp
│       ├── ufe_cpp_fields_fix44.hpp
│       ├── ufe_cpp_fields_fix50.hpp
│       ├── ufe_cpp_fields_fix50sp1.hpp
│       ├── ufe_cpp_fields_fix50sp2.hpp
│       ├── ufeapi.pb.h
│       ├── ufeconfiguration.hpp
│       ├── ufeconsts.hpp
│       ├── ufecppdll.h
│       ├── ufecppversion.h
│       ├── ufeedclient.hpp
│       ├── ufeexception.hpp
│       └── ufemessage.hpp
├── lib
│   └── libufeedclient.so
└── samples
    ├── CMakeLists.txt
    └── sample0.cpp
```

# Interface

The main `UFEed_C++` interfaces/classes are `UFEMessage`,
`UFEMessage.::Builder` and `UFEedClient`. `UFEMessage` is read-only
accessor to underlying `WireMessage` object with mapped fields and
groups. `UFEMessage::Builder` is a helper class that follows "builder"
pattern to simplify C++ language constructs (i.e. setters return the
reference to an object it was called from):

```c++
auto login = _uc->create_message()
    .set_long_name("login")
    .set_type(WireMessage::st_system)
    .set_service_id(UFE_CMD_LOGIN)
    .add_field(UFE_CMD, UFE_CMD_LOGIN)
    .add_field(UFE_LOGIN_ID, "webuser")
    .add_field(UFE_LOGIN_PW, "5e8848");
// type of login is UFEMessage::Builder
```

## UFEMessage Builder

The `UFEMessage::Builder` class provides write-only acess to underlying
`WireMessage` object format utilised by the UFEGW. `UFEMessage::Builder`
ctors are public but in most cases `UFEedClient::create_message()`
method shall be used.

```c++
class Builder
{
public:
    /**
     * Ctor
     * \param wm WireMessage to init from NOT taking ownership
     */
    explicit Builder(WireMessage* wm): _wm(wm, WireMessageDeleter(false)) {}
    /**
     * Ctor
     * \param wm WireMessage to take ownership from
     */
    explicit Builder(WireMessagePtr&& wm): _wm(std::move(wm)) {}

    /// Returns service id
    int service_id() const;
    /// Sets service id, \return *this
    Builder& set_service_id(int service_id);
    /// Return sub service id
    int subservice_id() const;
    /// Sets subservice id, \return *this
    Builder& set_subservice_id(int subservice_id);
    /// Returns seq
    uint32_t seq() const;
    /// Sets seq, \return *this
    Builder& set_seq(uint32_t seq);
    /// Returns wire message type
    WireMessage::Type type() const;
    /// Sets wire message type, \return *this
    Builder& set_type(WireMessage::Type type);
    /// Returns message long name
    const std::string& long_name() const;
    /// Sets message long name
    Builder& set_long_name(std::string long_name);
    /// Return message name
    const std::string& name() const;
    /// Set message name, \return *this
    Builder& set_name(std::string name);
    /// Return inner WireMessage pointer
    const WireMessage* wire_message() const;

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
    Builder& add_field(uint32_t tag, T val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

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
    Builder& add_field(uint32_t tag, T val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds double field
     * \param tag double field tag
     * \param val double val
     * \param loc double location
     * \return *this
     */
    template<typename T, std::enable_if_t<
        std::is_same<T,  double>::value || std::is_same<T, float>::value
        > * = nullptr>
    Builder& add_field(uint32_t tag, T val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds string field
     * \param tag string field tag
     * \param val string value
     * \param loc string location
     * \return *this
     */
    Builder& add_field(uint32_t tag, const char* val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

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
    Builder& add_field(uint32_t tag, T&& val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds bool field
     * \param tag bool field tag
     * \param val bool value
     * \param loc bool location
     * \return *this
     */
    Builder& add_field(uint32_t tag, bool val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds time field
     * \param tag time field tag
     * \param val time value
     * \param loc time location
     * \return *this
     */
    Builder& add_field(uint32_t tag, TimePoint&& val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds UUID field
     * \param tag uuid tag
     * \param val uuid string representation
     * \param loc uuid location
     * \return *this
     */
    Builder& add_field(uint32_t tag, const Uuid& val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Adds status field
     * \param tag status tag
     * \param val status value
     * \param loc status location
     * \return *this
     */
    Builder& add_field(uint32_t tag, Status val, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /**
     * Added a set of fields to message
     * \tparam Iter iterator type
     * \param first begin iterator
     * \param last end iterator
     * \return *this
     */
    template<typename Iter>
    Builder& add_fields(Iter first, Iter last);

    /**
     * Adds group field to the message
     * \param to field to add to
     * \param tr group creation lambda
     * \param loc status location
     * \return created message unique_ptr ref
     */
    Builder& add_group(uint32_t tag, WireMessageGroup* &grp, const std::function<void(Builder&, WireMessageGroup*)>& tr={}, UFEField::UFEFieldLocation loc = UFEField::fl_body);

    /*! Add group item
     * \param grp wire message group pointer
     * \param grp_cont ufe message group container
     * \param location field location
     * \return *this
     */
    Builder add_group_item(WireMessageGroup* grp, UFEField::UFEFieldLocation location = UFEField::fl_body);

    UFEMessagePtr build();
};
```

`UFEMessage::Builder` usage sample:

```c++
auto login = _uc->create_message()
    .set_long_name("login")
    .set_type(WireMessage::st_system)
    .set_service_id(UFE_CMD_LOGIN)
    .add_field(UFE_CMD, UFE_CMD_LOGIN)
    .add_field(UFE_LOGIN_ID, "webuser")
    .add_field(UFE_LOGIN_PW, "5e8848");
```

`UFEMessage::Builder` creates `NewOrderSinge` message with groups:

```c++
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
            .set_long_name("NoAlloc")
            .set_type(WireMessage::st_fixmsg)
            .set_seq(1)
            .add_field(AllocAccount::tag, "ABC")
            .add_field(AllocQty::tag, 2);
        m.add_group_item(grp)
            .set_long_name("NoAlloc")
            .set_type(WireMessage::st_fixmsg)
            .set_seq(2)
            .add_field(AllocAccount::tag, "CDE")
            .add_field(AllocQty::tag, 4);
    });
```

## UFEMessage

The `UFEMessage` class provides read-only access to underlying
`WireMessage` object format utilised by the UFEGW. `UFEMessage` ctors
are public but in most cases `UFEMessage::Builder::build()` method shall
be used to extend `UFEMessage` class to other underlying protocols and
middleware plugins in the future. `UFEMessage` has no copy ctor as well
as no assignment operator - use `msg->clone()` to create a message copy.

```c++
class UFEMessage
{
public:
    /**
     * Ctors
     */
    UFEMessage(std::string long_name, WireMessage::Type type, int service_id, WireMessagePtr&& wm);
    explicit UFEMessage(WireMessage* wm);
    explicit UFEMessage(WireMessagePtr&& wm);

    /**
     * Clone UFE message
     * \return clonned message unique ptr
     */
    UFEMessagePtr clone();

    /// Returns service id
    int service_id() const;
    /// Sets service id, \return *this
    UFEMessage& service_id(int service_id);

    /// Return sub service id
    int subservice_id() const;
    /// Sets subservice id, \return *this
    UFEMessage& subservice_id(int subservice_id);

    /// Returns seq
    uint32_t seq() const;
    /// Sets seq, \return *this
    UFEMessage& seq(uint32_t seq);

    /// Returns wire message type
    WireMessage::Type type() const;
    /// Sets wire message type, \return *this
    UFEMessage& type(WireMessage::Type type);

    /// Returns message long name
    const std::string& long_name() const;

    /// Return message name
    const std::string& name() const;
    /// Set message name, \return *this
    UFEMessage& name(std::string name);

    /// Gets mapped fields, \return field map
    const FieldsMap& fields() const;
    /// Gets mapped groups, \return group map
    const GroupsMap& groups() const;
    /// Gets wire mesage, \return wire message
    const WireMessage* wire_message() const;

    /**
     * Finds field
     * \param tag field tag to find
     * \return UFEField pointer if found, otherwise nullptr
     */
    const UFEField* find_field(uint32_t tag) const;
    UFEField* find_field(uint32_t tag);
    template<typename T>
    std::optional<T> find_field_value(uint32_t tag) const;

    /**
     * Finds group
     * \param tag group tag to find
     * \return GroupsContainer pointer if found, otherwise nullptr
     */
    const GroupsContainer* find_group(uint32_t tag) const;

    UFEMessage::Builder new_builder();
```

`UFEMessage` usage sample:

```c++ {.syntaxhighlighter-pre syntaxhighlighter-params="brush: cpp; gutter: false; theme: RDark" theme="RDark"}
auto login = _uc->create_message()
    .set_long_name("login")
    .set_type(WireMessage::st_system)
    .set_service_id(UFE_CMD_LOGIN)
    .add_field(UFE_CMD, UFE_CMD_LOGIN)
    .add_field(UFE_LOGIN_ID, "webuser")
    .add_field(UFE_LOGIN_PW, "5e8848")
    .build();       // build Message from Message::Builder
auto const * login_id = login->find_field(UFE_LOGIN_ID);
auto login_pw = login->find_field_value(UFE_LOGIN_PW);
assert(login_pw.has_value());
```

## UFEedClient

The `UFEedClient` class is used as the interface to make both System and
Business API calls to the UFEGW. Sessions between `UFEedClient` and the
UFEGW are made up of ZeroMQ PUB/SUB and REQ/REP sockets. The network
addresses and message topics inherent to these sockets are configurable
via `UFEedClient`. In addition, the `UFEedClient` manages these UFEGW
sessions on behalf of the user (after the user has successfully logged
in).

`UFEedClient` provides a callback interface called `Listener` that must
be implemented by `UFEedClient` consumer:

```c++
struct Listener
{
    /**
      * Called when subscription message received, PUB/SUB pattern
      * \param message received subscription message
      */
      virtual void subscription_message_received(const UFEMessagePtr& message) = 0;
    /**
      * Called when responder message received, back cnannel pattern
      * \param message received responder message
      */
      virtual void responder_message_received(const UFEMessagePtr& message) = 0;
    /**
      * Called when response message received, REQ/REP pattern
      * \param message received response message
      */
    virtual void response_message_received(const UFEMessagePtr& message) = 0;
    /**
      * Called when authentication is requested via back channel
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
```

`UFEedClient` is configured with `UFEconfiguration` class:

```c++
class UFEedConfiguration
{
public:
    /**
    * Subscriber endpoint, defaults to "tcp://127.0.0.1:55745"
    * \return subscriber endpoint
    */
    std::string subscriber() const;
    UFEedConfiguration& subscriber(std::string subscriber);

    /**
    * Requester endpoint, defaults to "tcp://127.0.0.1:55746"
    * \return subscriber endpoint
    */
    std::string requester() const;
    UFEedConfiguration& requester(std::string requester);

    /**
    * Publisher endpoint, defaults to "tcp://\*:55747"
    * \return publisher endpoint
    */
    std::string publisher() const;
    UFEedConfiguration& publisher(std::string publisher);

    /**
    * Responder endpoint, defaults to "tcp://\*:55748"
    * \return responder endpoint
    */
    std::string responder() const;
    UFEedConfiguration& responder(std::string responder);

    /**
    * Subscriber topic, defaults to "ufegw-publisher"
    * \return subscriber topic
    */
    std::string subscriber_topic() const;
    UFEedConfiguration& subscriber_topic(std::string subscriber_topic);

    /**
    * Requester topic, defaults to "ufegw-responder"
    * \return requester topic
    */
    std::string requester_topic() const;
    UFEedConfiguration& requester_topic(std::string requester_topic);

    /**
    * Publisher topic, defaults to "ufeedclient-publisher"
    * \return publisher topic
    */
    std::string publisher_topic() const;
    UFEedConfiguration& publisher_topic(std::string publisher_topic);

    /**
    * Responder topic, defaults to "ufeedclient-responder"
    * \return responder topic
    */
    std::string responder_topic() const;
    UFEedConfiguration& responder_topic(std::string responder_topic);

    /**
    * Max zmq IO thread count, defaults to 1
    * \return max zmq IO thread count
    */
    uint32_t max_io_threads() const;
    UFEedConfiguration& max_io_threads(uint32_t max_io_threads);

    /**
    * Poll interval in msecs, defaults to 10 ms
    * \return poll interval in msecs
    */
    uint64_t poll_interval_ms() const;
    UFEedConfiguration& poll_interval_ms(uint64_t poll_interval_ms);
};
```

`UFEedClient` interface:

```c++
class UFEedClient
{
public:
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
     * Creates UFEMessage factory methods
     * \param long_name message long name
     * \param type message type, \see WireMessage::Type
     * \param service_id message service id
     * \return message builder. \see UFEMessage::Builder
     */
    UFECPPAPI UFEMessage::Builder create_message();
    UFECPPAPI UFEMessage::Builder create_message(WireMessagePtr&& wm);

    /**
     * Synchronously sends request to UFEGW and waits for UFE response, REQ/REP pattern
     * \param request request to send. Ownership of UFEMessage unique ptr IS taken. \see UFEMessage
     * \return
     */
    UFECPPAPI UFEMessagePtr request(UFEMessage::Builder&& request);
    /**
     * Send message to responder channel. Back channel patern.
     * \param msg request to send. Ownership of UFEMessage unique ptr IS taken. \see UFEMessage
     */
    UFECPPAPI void respond(UFEMessage::Builder&& msg);
};
```

`UFEedClient` usage sample:

```c++
 struct Listener: public UFEedClient::Listener {...} listener;
 auto _uc = make_shared<UFEedClient>(UFEedConfiguration()
    .max_io_threads(2)
    .poll_interval_ms(1)
    .subscriber(SUBSCRIBER_DEFAULT)
    .responder_topic("ufegw-requester")
 , &listener);
 _uc->connect();
 // logon
 auto login = _uc->create_message();
 login->set_long_name("login")
    .set_type(WireMessage::st_system)
    .set_service_id(UFE_CMD_LOGIN)
    .add_field(UFE_CMD, UFE_CMD_LOGIN)
    .add_field(UFE_LOGIN_ID, "abcdef")
    .add_field(UFE_LOGIN_PW, "5e8848");
 auto response = _uc->request(move(login));
 ...
 _uc->stop();
 _uc.reset();
```

# Constants

The `UFEed_C++` maintains a list of constant values that translate to integer
codes in the UFEGW. These integer codes are used to identify System API
services as well as general FIX functionality. A full list of these
constants is available under `include/ufecpp/ufeconsts.hpp`.

## FIX variants constants

The `UFEed_C++` provides constants for all stock FIX variants:

```c++
#include <ufecpp/ufe_cpp_fields_fix50sp2.hpp>
...
// NOS creation
using namespace FIX50SP2::Field;
auto nos = _uc->create_message()
    .set_long_name("NewOrderSingle")
    .set_type(WireMessage::st_fixmsg)
    .set_service_id(1)
    .set_name(MsgType::NEWORDERSINGLE)
    .add_field(ClOrdID::tag, "123")
    .add_field(TransactTime::tag, std::chrono::system_clock::now())
    .add_field(ExecInst::tag, ExecInst::ALL_OR_NONE)
    .add_field(OrdType::tag, OrdType::LIMIT)
    .add_field(Side::tag, Side::BUY);
```

# Building

The `UFEed_C++` build is CMake based. To build use the following command line:
```shell
mkdir .build
cd .build
cmake ..
make -j4
```

`UFEed_C++` cand be used with address sanitizer by setting `-DUFECPP_ENABLE_ADDRESS_SANITIZER=ON` at cmake command line. To specify cmake install directory use standard cmake approach by setting `-DCMAKE_INSTALL_PREFIX=<install_root>` at cmake command line

The `UFEed_C++` provides a simple sample to use as a starting point for UFEed
C++ development. The sample is under samples folder. To build sample,
you have to set `UFEed_C++` installation root directory as a cmake command line
parameter:
```shell
cd samples
mkdir .build
cd .build
cmake -DUFECPP_ROOT=<UFEed_C++ install dir> ..
make -j4
```
