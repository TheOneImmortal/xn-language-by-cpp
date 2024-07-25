#pragma once
/**
 * ****************useful_macro.hh****************
 * @brief  方便地宏
 * @author https://github.com/TheOneImmortal
 * @date   2024-06-24
 * ********************************
 */

/*----------------include----------------*/

/*----------------body----------------*/

#define XN_USEFUL_MACRO__CONCEPT2(target)                   \
	template<typename T>                                    \
	constexpr bool is_##target##_v = is_##target<T>::value; \
                                                            \
	template<typename T>                                    \
	concept target##_c = is_##target##_v<T>;
