#pragma once

/**
 * ****************unpacking.hh****************
 * @brief  解包器
 * @author https://github.com/TheOneImmortal
 * @date   2024-06-23
 * ********************************
 */

/*----------------include----------------*/
#include <type_traits>

/*----------------body----------------*/

namespace xn {

namespace internal {
	template<typename T>
	struct is_could_be_unpacking: ::std::false_type {};

#define _XN_UNPACKING_HELPER(count, name...)                    \
	template<typename T>                                        \
	struct UnpackingHelper##count {                             \
		UnpackingHelper##count(T const &v) { auto [name] = v; } \
	};                                                          \
                                                                \
	template<typename T>                                        \
	concept could_be_unpacking##count = requires(T v) {         \
		UnpackingHelper##count<T>(v);                           \
	}

	_XN_UNPACKING_HELPER(1, a);
	_XN_UNPACKING_HELPER(2, a, b);
	_XN_UNPACKING_HELPER(3, a, b, c);
	_XN_UNPACKING_HELPER(4, a, b, c, d);
	_XN_UNPACKING_HELPER(5, a, b, c, d, e);
	_XN_UNPACKING_HELPER(6, a, b, c, d, e, f);

	template<typename T>
	    requires(could_be_unpacking1<T> || could_be_unpacking2<T>
	             || could_be_unpacking3<T> || could_be_unpacking4<T>
	             || could_be_unpacking5<T> || could_be_unpacking6<T>)
	struct is_could_be_unpacking<T>: ::std::true_type {};
}  //namespace internal

template<typename T>
using is_could_be_unpacking = internal::is_could_be_unpacking<T>;

template<typename T>
constexpr bool is_could_be_unpacking_v = is_could_be_unpacking<T>::value;

template<typename T>
concept could_be_unpacking = is_could_be_unpacking_v<T>;

}  //namespace xn
