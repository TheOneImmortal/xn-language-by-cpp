/**
 * ****************type_traits_extension.hh****************
 * @brief  <type_traits>扩展
 * @author https://github.com/TheOneImmortal
 * @date   2024-03-14
 * ********************************
 */

#ifndef __XN_BT__TYPE_TRAITS_EXTENSION_HH__
#define __XN_BT__TYPE_TRAITS_EXTENSION_HH__

/*----------------include----------------*/

/*----------------body----------------*/
namespace std {

template<typename T>
struct remove_member_pointer {
	using type = T;
};

template<typename Class, typename T>
struct remove_member_pointer<T Class::*> {
	using type = T;
};

template<typename T>
using remove_member_pointer_t = typename remove_member_pointer<T>::type;

}

#endif
