/**
 * ****************type_conversion.hh****************
 * @brief  方便的类型转换函数
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__TYPE_CONVERSION_HH__
#define __XN_BT__TYPE_CONVERSION_HH__

/*----------------include----------------*/
#include <type_traits>

/*----------------body----------------*/
namespace xn {

[[nodiscard]] constexpr auto as_void(auto &&v) noexcept {
	using namespace std;
	if constexpr (using T = remove_reference_t<decltype(v)>;
	              is_pointer_v<T>) {
		if constexpr (using T = remove_pointer_t<T>; is_const_v<T>)
			return (void const *) v;
		else return (void *) v;
	} else if constexpr (is_const_v<T>) {
		return (void const *) &v;
	} else return (void *) &v;
}

template<class T>
[[nodiscard]] constexpr auto as_pointer(auto &&v) noexcept {
	if constexpr (std::is_const_v<
	                  std::remove_pointer_t<decltype(as_void(v))>>) {
		return (T const *) as_void(v);
	} else return (T *) (as_void(v));
}

}  // namespace xn

#endif
