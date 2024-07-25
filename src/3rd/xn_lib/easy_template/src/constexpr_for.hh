/**
 * ****************constexpr_for.hh****************
 * @brief  编译器for展开
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-10
 * ********************************
 */

#ifndef __XN_ET__CONSTEXPR_FOR_HH__
#define __XN_ET__CONSTEXPR_FOR_HH__

/*----------------include----------------*/
#include <type_traits>
#include <utility>

#include "../../base_type/include_me.hh"

/*----------------body----------------*/
namespace xn {

template<typename F>
concept loop_callable = ::std::invocable<F> || requires(F &&f) {
	f.template operator()<0>();
};
template<typename Tp, typename F>
concept tuple_loop_callable1 = requires(F &&f) {
	f.template operator()<::std::tuple_element_t<0, Tp>>();
};
template<typename Tp, typename F>
concept tuple_loop_callable2 = requires(F &&f) {
	f.template operator()<0, ::std::tuple_element_t<0, Tp>>();
};
template<typename Tp, typename F>
concept tuple_loop_callable
    = tuple_loop_callable1<Tp, F> || tuple_loop_callable2<Tp, F>;

template<u64 N>
constexpr auto loop(loop_callable auto &&f) {
	if constexpr (::std::invocable<decltype(f)>)
		[&]<u64... I>(::std::index_sequence<I...>) {
			auto _ = ((f(), I), ...);
		}(::std::make_index_sequence<N>{});
	else
		[&]<u64... I>(::std::index_sequence<I...>) {
			(f.template operator()<I>(), ...);
		}(::std::make_index_sequence<N>{});
}

template<typename Tp, typename F>
    requires(tuple_loop_callable<Tp, F>&& requires {
	    ::std::make_index_sequence<::std::tuple_size_v<Tp>>{};
    })
constexpr auto loop(F &&f) {
	if constexpr (tuple_loop_callable1<Tp, F>)
		[&]<u64... I>(::std::index_sequence<I...>) {
			((f.template operator()<::std::tuple_element_t<I, Tp>>()),
			 ...);
		}(::std::make_index_sequence<::std::tuple_size_v<Tp>>{});
	else
		[&]<u64... I>(::std::index_sequence<I...>) {
			((f.template operator()<I, ::std::tuple_element_t<I, Tp>>()),
			 ...);
		}(::std::make_index_sequence<::std::tuple_size_v<Tp>>{});
}

template<typename Sq>
    requires requires() {
	    []<typename Enum, Enum... I>(
	        ::std::type_identity<::std::integer_sequence<Enum, I...>>) {
	    }(::std::type_identity<Sq>{});
    }
constexpr auto loop(auto &&f) {
	[&]<typename Enum, Enum... I>(
	    ::std::type_identity<::std::integer_sequence<Enum, I...>>) {
		(f.template operator()<I>(), ...);
	}(::std::type_identity<Sq>{});
}

}  // namespace xn

#endif
