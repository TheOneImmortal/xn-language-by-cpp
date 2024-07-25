#pragma once
/**
 * ****************useful_macro.hh****************
 * @brief  好用的宏
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-16
 * ********************************
 */

/*----------------include----------------*/

/*----------------body----------------*/

#define visit__start() \
::std::visit(                           \
    [&](auto &&item) noexcept -> void { \
	    using T = ::std::decay_t<decltype(item)>
#define if_T_is(type) if constexpr (::std::is_same_v<T, type>)

#define elif_T_is(type) else if_T_is(type)

#define visit__end(variant) \
	}, variant)
