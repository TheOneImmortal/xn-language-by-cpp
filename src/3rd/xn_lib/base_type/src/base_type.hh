#ifndef __XN__BASETYPEDEF_HH__
#define __XN__BASETYPEDEF_HH__

#include <limits>  // IWYU pragma: keep
#include <string_view>
#include <type_traits>
#include <utility>

#include "./cencept_extension.hh"  // IWYU pragma: keep

#undef max

using i8    = int8_t;
using i16   = int16_t;
using i32   = int32_t;
using i64   = int64_t;
using isize = long;

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using usize = unsigned long;

using f32 = float;
using f64 = double;

using wchar = wchar_t;
using utf8  = char8_t;
using utf16 = char16_t;
using utf32 = char32_t;

template<xn::num_c T>
struct v2 {
	T x, y;
};

using string = ::std::string;
using str    = ::std::string_view;

namespace xn {
using ::std::forward;
using ::std::move;

template<u64 i>
struct smallest_uint;

template<u64 i>
    requires(0 < i) && (i <= ::std::numeric_limits<u8>::max())
struct smallest_uint<i>: ::std::type_identity<u8> {};

template<u64 i>
    requires(::std::numeric_limits<u8>::max() < i)
         && (i <= ::std::numeric_limits<u16>::max())
struct smallest_uint<i>: ::std::type_identity<u16> {};

template<u64 i>
    requires(::std::numeric_limits<u16>::max() < i)
         && (i <= ::std::numeric_limits<u32>::max())
struct smallest_uint<i>: ::std::type_identity<u32> {};

template<u64 i>
    requires(::std::numeric_limits<u32>::max() < i)
         && (i <= ::std::numeric_limits<u64>::max())
struct smallest_uint<i>: ::std::type_identity<u64> {};

template<u64 i>
using smallest_uint_t = smallest_uint<i>::type;
}

#define var auto
#define val auto const

#endif
