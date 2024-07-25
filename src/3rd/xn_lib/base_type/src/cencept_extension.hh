/**
 * ****************cencept_extension.hh****************
 * @brief  对标准库约束进行扩展
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__CENCEPT_EXTENSION_HH__
#define __XN_BT__CENCEPT_EXTENSION_HH__

/*----------------include----------------*/
#include <concepts>  // IWYU pragma: keep
#include <type_traits>

/*----------------body----------------*/
namespace xn {

template<typename T>
concept num_c = std::is_arithmetic_v<T> && !std::is_pointer_v<T>;

template<typename T>
concept int_c = std::integral<T>;

template<typename T>
concept easy_type_c
    = ::std::is_trivial_v<T> && ::std::is_standard_layout_v<T>
   && !::std::is_pointer_v<T>;

}

#endif
