/**
 * ****************format_adaptation.hh****************
 * @brief  适配format
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__FORMAT_ADAPTATION_HH__
#define __XN_BT__FORMAT_ADAPTATION_HH__

/*----------------include----------------*/
#include <format>
#include <vector>

#include "base_type.hh"

/*----------------body----------------*/
namespace std {

#define FORMATTER_NO_PARSE                               \
	constexpr auto parse(format_parse_context const &pc) \
	    const noexcept {                                 \
		auto end = ranges::find(pc, '}');                \
		return end;                                      \
	}

template<xn::num_c T>
struct formatter<v2<T>> {
	FORMATTER_NO_PARSE

	auto format(v2<T> const &v, format_context &fc) const noexcept {
		return format_to(fc.out(), "{}:{}", v.x, v.y);
	}
};

template<>
struct formatter<::std::vector<::std::byte>> {
	FORMATTER_NO_PARSE

	auto format(::std::vector<::std::byte> const &v, format_context &fc)
	    const noexcept {
		for (auto const &B : v) format_to(fc.out(), "{:02x} ", (u8) B);
		return fc.out();
	}
};

}  // namespace std

#endif
