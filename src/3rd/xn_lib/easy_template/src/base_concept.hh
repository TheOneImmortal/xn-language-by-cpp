/**
 * ****************base-concept.hh****************
 * @brief  基础约束
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-10
 * ********************************
 */

#ifndef __XN_ET__BASE_CONCEPT_HH__
#define __XN_ET__BASE_CONCEPT_HH__

/*----------------include----------------*/
#include <type_traits>

/*----------------body----------------*/
namespace xn {

/**
 * @brief 是否 T 是 衰退类型
 *
 * @tparam T 被判断类型
 */
template<typename T>
concept decay_c = requires() {
	requires std::is_same_v<T, std::decay<T>>;
};

}  // namespace xn

#endif
