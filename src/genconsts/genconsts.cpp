#include <cxxopts.hpp>
#include <ctime>
#include <string>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

void generate_consts(const string& fix_path, const string& ufe_path, const string& out_path)
{
	ofstream out(out_path);
	auto now{ system_clock::to_time_t(system_clock::now()) };
	out << "// THIS FILE HAS BEEN AUTOGENERATED BY genconsts ON " << ctime(&now)
		 << R"xx(
#ifndef UFE_CONSTS_03847beb_df7c_43fe_94ed_a9978b5f4fc2_
#define UFE_CONSTS_03847beb_df7c_43fe_94ed_a9978b5f4fc2_

#include <string>
#include <string_view>
#include <ufecpp/ufecppversion.h>

namespace UFECPP
{
	using namespace std::string_view_literals;

	// UFE CONSTS
)xx";

	auto to_upper = [](std::string s) -> string
	{
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
		return s;
	};

	unordered_map<string, int> ufe_offsets;
	const std::regex
		r1(R"xx(constexpr\s*int64_t\s*(.*)\(.*([0-9]{5});.*)xx"),
		r2(R"xx(const int64_t (\S*)\s*\{\s*(\d+)\s.*)xx"),
		r3(R"xx(const int64_t (\S*)\s*\{\s*(0[xX][0-9a-fA-F]+)\s.*)xx"),
		r4(R"xx(const int64_t (\S*)\s*\{\s*(.*)\(([0-9]*)\).*)xx"),
		r5(R"xx(const .* (Common.*)\((\S*)\).*)xx");
	std::smatch m1, m2, m3, m4, m5;

	ifstream ufe(ufe_path);
	while(!ufe.eof())
	{
		string line;
		getline(ufe, line);

		//eg. constexpr int64_t fix8_status(int64_t src) { return src + 70000; }
		if(regex_match(line, m1, r1) && !m1.empty())
			ufe_offsets[m1[1]] = stoi(m1[2]);

		//eg. const int64_t session_flag_report { 0 };
		if(regex_match(line, m2, r2) && !m2.empty())
			out << "\tconstexpr uint32_t " << to_upper(m2[1]) << " { " << m2[2] << " };" << endl;

		//eg. const int64_t ufe_all_services { 0x1ffff };
		if(regex_match(line, m3, r3) && !m3.empty())
			out << "\tconstexpr uint32_t " << to_upper(m3[1]) << " { " << m3[2] << " };" << endl;

		//eg. const int64_t fix8_ok { fix8_status(0) };
		if (regex_match(line, m4, r4) && !m4.empty())
		{
			auto offset = ufe_offsets[m4[2]];
			out << "\tconstexpr uint32_t " << to_upper(m4[1]) << " { " << offset + stoi(m4[3]) << " };" << endl;
		}
	}
	auto offset1 = ufe_offsets["ufe_common_fix"];
	out << "\tconstexpr uint32_t UFE_COMMON_FIX { " << offset1 << " };" << endl;

	//Write UFEGW connection defaults
	out << R"xx(
	constexpr int UFE_FLOAT_PRECISION { 2 };

	// UFEGW CONSTS
	constexpr std::string_view SUBSCRIBER { "subscriber"sv };
	constexpr std::string_view SUBSCRIBER_DEFAULT { "tcp://127.0.0.1:55745"sv };
	constexpr std::string_view REQUESTER { "requester"sv };
	constexpr std::string_view REQUESTER_DEFAULT { "tcp://127.0.0.1:55746"sv };
	constexpr std::string_view PUBLISHER { "publisher"sv };
	constexpr std::string_view PUBLISHER_DEFAULT { "tcp://*:55747"sv };
	constexpr std::string_view RESPONDER { "responder"sv };
	constexpr std::string_view RESPONDER_DEFAULT { "tcp://*:55748"sv };
	constexpr std::string_view SUBSCRIBER_TOPIC { "subscribertopic"sv };
	constexpr std::string_view SUBSCRIBER_TOPIC_DEFAULT { "ufegw-publisher"sv };
	constexpr std::string_view REQUESTER_TOPIC { "requestertopic"sv };
	constexpr std::string_view REQUESTER_TOPIC_DEFAULT { "ufegw-responder"sv };
	constexpr std::string_view PUBLISHER_TOPIC { "publishertopic"sv };
	constexpr std::string_view PUBLISHER_TOPIC_DEFAULT { "ufeedclient-publisher"sv };
	constexpr std::string_view RESPONDER_TOPIC { "respondertopic"sv };
	constexpr std::string_view RESPONDER_TOPIC_DEFAULT { "ufeedclient-responder"sv };

	// FIX CONSTS
)xx";

	ifstream fix(fix_path);
	while(!fix.eof())
	{
		string line;
		getline(fix, line);

		//eg. const f8String Common_MsgType_HEARTBEAT("0");
		if (regex_match(line, m5, r5))
		{
			auto name = to_upper(m5[1]);
			if (line.find("f8String") != string::npos)
				out << "\tconstexpr std::string_view " << name << " { " << m5[2] << "sv };" << endl;
			else if (line.find("char") != string::npos)
				out << "\tconstexpr char " << name << " { " << m5[2] << " };" << endl;
			else
				out << "\tconstexpr uint32_t " << name << " { " << m5[2] << " };" << endl;
		}
	}

	out << R"xx(
}

#endif // UFE_CONSTS_03847beb_df7c_43fe_94ed_a9978b5f4fc2_

)xx";
}

int main(int argc, char **argv)
{
	try
	{
		cxxopts::Options options(argv[0], " _ genconsts cmd line options");
		string fix_path, ufe_path, out_path;
		options
			.add_options()
				("f,fix", "Set FIX fields path to fixcommon.hpp, defaults to fixcommon.hpp", cxxopts::value<string>(fix_path)->default_value("field.hpp"))
				("u,ufe", "Set UFE fields path to ufeconsts.hpp, defaults to ufeconsts.hpp", cxxopts::value<string>(ufe_path)->default_value("ufeconsts.hpp"))
				("o,out", "Set C# output path to Consts.cs, defaults to ufeconsts.hpp", cxxopts::value<string>(out_path)->default_value("ufeconsts.hpp"))
				("h,help", "Prints help")
			;
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			return 0;
		}
		generate_consts(fix_path, ufe_path, out_path);
	}
	catch(const cxxopts::OptionException& e)
	{
		std::cout << "error parsing options: " << e.what() << "\ntry " << argv[0] << " --help" << std::endl;
		return -1;
	}
	return 0;
}
