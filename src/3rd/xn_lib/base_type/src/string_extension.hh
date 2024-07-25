/**
 * ****************string_extension.hh****************
 * @brief  字符串扩展
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__STRING_EXTENSION_HH__
#define __XN_BT__STRING_EXTENSION_HH__

/*----------------include----------------*/
#include <format>
#include <iostream>

#include "base_type.hh"

/*----------------body----------------*/
namespace xn {

using std::vformat;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-stack-address"

consteval auto operator""_f(char const *s, u64) {
	return [=]<typename... T>(T &&...arg) noexcept -> str {
		return vformat(s, std::make_format_args(xn::forward<T>(arg)...));
	};
}

consteval auto operator""_fln(char const *s, u64) {
	return [=]<typename... T>(T &&...arg) noexcept -> str {
		return vformat(s, std::make_format_args(xn::forward<T>(arg)...))
		     + '\n';
	};
}

#pragma clang diagnostic pop

#if 0

// 当前代码，暂不支持
template<std::template_string s>
constexpr auto operator""_out() {
    return []<class... T>(T... args) noexcept {
        std::print(s.str, args...);
    };
}

template<std::template_string s>
constexpr auto operator""_outln() {
    return []<class... T>(T... args) noexcept {
        std::println(s.str, args...);
    };
}

#else

consteval auto operator""_out(char const *s, u64) {
	return [=]<typename... T>(T &&...arg) noexcept {
		std::cout
		    << vformat(s, std::make_format_args(xn::forward<T>(arg)...));
	};
}

consteval auto operator""_outln(char const *s, u64) {
	return [=]<typename... T>(T &&...arg) noexcept {
		std::cout
		    << vformat(s, std::make_format_args(xn::forward<T>(arg)...))
		    << '\n';
	};
}

#endif

}  // namespace xn

#endif
