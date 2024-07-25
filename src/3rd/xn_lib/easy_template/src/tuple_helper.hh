#pragma once
/**
 * ****************tuple_helper.hh****************
 * @brief  元组功能扩展
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-14
 * ********************************
 */

/*----------------include----------------*/
#include <tuple>
#include <type_traits>
#include <utility>

#include "../../base_type/include_me.hh"
#include "useful_macro.hh"

/*----------------body----------------*/
namespace xn {

template<class T>
struct is_tuple: ::std::false_type {};

template<class... Ts>
struct is_tuple<::std::tuple<Ts...>>: ::std::true_type {};

XN_USEFUL_MACRO__CONCEPT2(tuple)

template<class... Tps>
struct combine_tuples
    : ::std::type_identity<decltype(::std::tuple_cat<Tps...>(Tps()...))> {
};
template<class... Tps>
using combine_tuples_t = combine_tuples<Tps...>::type;

template<class Tp>
struct tuple_byte_size;

template<typename... Ts>
struct tuple_byte_size<::std::tuple<Ts...>>
    : ::std::integral_constant<u64, [] {
	    return ((::std::is_empty_v<Ts> ? 0 : sizeof(Ts)) + ...);
    }()> {};

template<class Tp>
constexpr u64 tuple_byte_size_v = tuple_byte_size<Tp>::value;

template<class Tp, typename T>
struct tuple_index_of;

template<typename... Ts, typename T>
struct tuple_index_of<::std::tuple<Ts...>, T>
    : ::std::integral_constant<u64, [] {
	    using Tp  = ::std::tuple<Ts...>;
	    u64 index = ::std::tuple_size_v<Tp>;

	    loop<Tp>([&]<u64 i, typename Ti> {
		    if constexpr (::std::is_same_v<T, Ti>) index = i;
	    });

	    return index;
    }()> {};

template<class Tp, typename T>
constexpr u64 tuple_index_of_v = tuple_index_of<Tp, T>::value;

namespace detail {

	template<typename T, typename... Ts>
	constexpr bool is_no_same() {
		if constexpr (sizeof...(Ts) == 0) return true;
		else if constexpr (!is_no_same<Ts...>()) return false;
		else return !(::std::is_same_v<T, Ts> || ...);
	}

	template<class Tp>
	struct is_no_same_tp;

	template<class... Ts>
	struct is_no_same_tp<::std::tuple<Ts...>>
	    : ::std::integer_sequence<bool, [] {
		    if constexpr (sizeof...(Ts) == 0) return true;
		    else return is_no_same<Ts...>();
	    }()> {};

};

template<typename... Ts>
struct is_no_same: ::std::integral_constant<bool, [] {
	if constexpr (sizeof...(Ts) == 0) return true;
	else return detail::is_no_same<Ts...>();
}()> {};

template<typename... Ts>
constexpr bool is_no_same_v = is_no_same<Ts...>::value;

template<class Tp>
struct is_no_same_tp;

template<typename... Ts>
struct is_no_same_tp<::std::tuple<Ts...>>
    : ::std::integral_constant<bool, [] {
	    if constexpr (sizeof...(Ts) == 0) return true;
	    else return detail::is_no_same<Ts...>();
    }()> {};

XN_USEFUL_MACRO__CONCEPT2(no_same_tp)

}  // namespace xn
