#include <format>  // IWYU pragma: keep
#include <fstream>
#include <iostream>  // IWYU pragma: keep

#define XN_DEBUG_LOG_ENABLE 1

namespace xn::log {

inline ::std::fstream log_file{ "debug.log", ::std::ios::out };
inline ::std::fstream error_file{ "error.log", ::std::ios::out };

template<typename... Ts>
void debug_log(::std::string fstr, Ts... args) {
#if XN_DEBUG_LOG_ENABLE
	::std::cout << "[debug log]: "
	            << ::std::vformat(fstr,
	                              ::std::make_format_args(args...))
	            << ::std::endl;
	log_file << "[debug log]: "
	         << ::std::vformat(fstr, ::std::make_format_args(args...))
	         << ::std::endl;
#endif
}

template<typename... Ts>
void important_log(::std::string fstr, Ts... args) {
	::std::cout << "[----- log]: "
	            << ::std::vformat(fstr,
	                              ::std::make_format_args(args...))
	            << ::std::endl;
	log_file << "[----- log]: "
	         << ::std::vformat(fstr, ::std::make_format_args(args...))
	         << ::std::endl;
	error_file << "[----- log]: "
	           << ::std::vformat(fstr, ::std::make_format_args(args...))
	           << ::std::endl;
}

template<typename... Ts>
void warning_log(::std::string fstr, Ts... args) {
	::std::cout << "[warni log]: "
	            << ::std::vformat(fstr,
	                              ::std::make_format_args(args...))
	            << ::std::endl;
	log_file << "[warni log]: "
	         << ::std::vformat(fstr, ::std::make_format_args(args...))
	         << ::std::endl;
	error_file << "[warni log]: "
	           << ::std::vformat(fstr, ::std::make_format_args(args...))
	           << ::std::endl;
}

template<typename... Ts>
void error_log(::std::string fstr, Ts... args) {
	::std::cout << "[error log]: "
	            << ::std::vformat(fstr,
	                              ::std::make_format_args(args...))
	            << ::std::endl;
	log_file << "[error log]: "
	         << ::std::vformat(fstr, ::std::make_format_args(args...))
	         << ::std::endl;
	error_file << "[error log]: "
	           << ::std::vformat(fstr, ::std::make_format_args(args...))
	           << ::std::endl;
}

}  //namespace xn::log
