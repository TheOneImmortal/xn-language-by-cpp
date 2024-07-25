/**
 * ****************protocol.hh****************
 * @brief  [ID-长度-内容]封装协议
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_SU__PROTOCOL_HH__
#define __XN_SU__PROTOCOL_HH__

/*----------------include----------------*/
#include <array>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../base_type/include_me.hh"
#include "../../easy_template/include_me.hh"

/*----------------body----------------*/
namespace xn {

/**
 * @brief idl协议，也就是ID-长度-内容
 *
 * 信息总长u32 信息1类型uauto 信息1长度u16 信息1内容(信息1长度) 信息2...
 *
 * warning:
 * - idl协议本身如果知晓如何工作的话，其实相当于明文，请在外部自行加密
 * - 每条信息的长度不能超过65535，否则是ub
 */
namespace idl {
	namespace internal {

		enum class protocol_setting {
			// 默认使用Maximum
			Minumum,  // 最小化协议，即尽可能少使用长度字段，旧协议遇到新协议可能会崩溃，如果应用强版本约束可以使用这个用以最大化效率
			Maximum,  // 最大化协议，即尽可能多使用长度字段，旧协议遇到新协议会跳过暂不处理，如果应用想要版本兼容可以使用这个以提高兼容性（虽然做不到完全兼容）
		};

		enum class msg_setting {
			// 不设置的话使用默认值，库作者觉得哪个好用就用哪个
			// 当前默认Multiple
			Single,  // 只保留最新信息 信息长度==结构体大小
			Multiple,  // 保留每个信息 信息长度==n*结构体大小
			// 当结构体大小为0时，信息长度永远为0，但是启用Maximum-Multiple模式后，会通过重复使用
			// id-0-id-0-id-0-... 这种形式来表达多次
		};

		using count_t  = u16;
		using length_t = u32;

		template<protocol_setting... setting>
		using protocol_settings
		    = ::std::integer_sequence<protocol_setting, setting...>;

		template<msg_setting... setting>
		using settings
		    = ::std::integer_sequence<msg_setting, setting...>;

// 使用前需要先手动定义一个bool is_resetting = false;
#define XN_IDL_PROTOCOL__EXPEND_OR(__setting_type, __item, ...) \
	|| __s                                                      \
	        == __setting_type::__item##__VA_OPT__(              \
	            XN_IDL_PROTOCOL__EXPEND_OR(__setting_type, __VA_ARGS__))

#define XN_IDL_PROTOCOL__CHECK_RESETTING(__setting_type, __setting, ...) \
	{                                                                    \
		bool __is_setted = false;                                        \
		::xn::loop<::std::integer_sequence<                              \
		    __setting_type, setting...>>([&]<auto __s> {                 \
			if constexpr (__s                                            \
				          == __setting_type::__setting                   \
				              __VA_OPT__(XN_IDL_PROTOCOL__EXPEND_OR(     \
				                  __setting_type, __VA_ARGS__))) {       \
				if (__is_setted) is_resetting = true;                    \
				else __is_setted = true;                                 \
			}                                                            \
		});                                                              \
	}

		template<msg_setting... setting>
		constexpr bool is_no_resetting() {
			bool is_resetting = false;
			XN_IDL_PROTOCOL__CHECK_RESETTING(msg_setting, Single, Multiple)
			return !is_resetting;
		}

		template<protocol_setting... setting>
		constexpr bool is_protocol_no_resetting() {
			bool is_resetting = false;
			XN_IDL_PROTOCOL__CHECK_RESETTING(protocol_setting, Minumum,
			                                 Maximum)
			return !is_resetting;
		}

#undef XN_IDL_PROTOCOL__CHECK_RESETTING

// 默认false
#define XN_IDL_PROTOCOL__BOOL_SETTING(__setting_type, __true_setting, \
                                      __false_setting)                \
	[] constexpr -> bool {                                            \
		bool __is_setted       = false;                               \
		bool __is_true_setting = false;                               \
		loop<std::integer_sequence<                                   \
			__setting_type, setting...>>([&]<__setting_type s> {      \
			if constexpr (s == __setting_type::__true_setting         \
				          || s == __setting_type::__false_setting) {  \
				__is_setted = true;                                   \
				__is_true_setting                                     \
					= (s == __setting_type::__true_setting);          \
			}                                                         \
		});                                                           \
                                                                      \
		if (!__is_setted) return false;                               \
		return __is_true_setting;                                     \
	}();

		template<protocol_setting... setting>
		struct make_protocol_setts {
			static constexpr bool is_minumum
			    = XN_IDL_PROTOCOL__BOOL_SETTING(protocol_setting,
			                                    Minumum, Maximum)
		};

		template<msg_setting... setting>
		struct make_setts {
			static constexpr bool is_single
			    = XN_IDL_PROTOCOL__BOOL_SETTING(msg_setting, Single,
			                                    Multiple)
		};

#undef XN_IDL_PROTOCOL__BOOL_SETTING

		template<typename T>
		struct is_msg_setting: public ::std::false_type {};

		template<msg_setting... setting>
		    requires(is_no_resetting<setting...>())
		struct is_msg_setting<settings<setting...>>
		    : public std::true_type {};

		XN_USEFUL_MACRO__CONCEPT2(msg_setting)

		template<typename T>
		struct is_protocol_settings: public ::std::false_type {};

		template<protocol_setting... setting>
		    requires(is_protocol_no_resetting<setting...>())
		struct is_protocol_settings<protocol_settings<setting...>>
		    : public ::std::true_type {};

		XN_USEFUL_MACRO__CONCEPT2(protocol_settings)

		template<typename T>
		struct is_makeing_item: public ::std::false_type {};

		template<msg_setting_c settings, easy_type_c... Ts>
		struct is_makeing_item<settings(Ts...)>
		    : public ::std::true_type {};

		XN_USEFUL_MACRO__CONCEPT2(makeing_item)

		template<makeing_item_c T>
		struct item;

		template<msg_setting... setting, typename... Ts>
		    requires(is_makeing_item_v<settings<setting...>(Ts...)>)
		struct item<settings<setting...>(Ts...)> {
		  public:
			using set   = make_setts<setting...>;
			using tuple = ::std::tuple<Ts...>;

			static constexpr u64 count = ::std::tuple_size_v<tuple>;
			static constexpr u64 size  = tuple_byte_size_v<tuple>;

			template<u64 I>
			using type = ::std::tuple_element_t<I, tuple>;
		};

		template<typename T>
		struct is_item: public ::std::false_type {};

		template<msg_setting... setting, typename... Ts>
		struct is_item<item<settings<setting...>(Ts...)>>
		    : public ::std::true_type {};

		XN_USEFUL_MACRO__CONCEPT2(item)

		template<typename T, makeing_item_c... Ts>
		    requires((is_protocol_settings_v<T>
		              && is_no_same_v<typename item<Ts>::tuple...>)
		             || (is_makeing_item_v<T>
		                 && is_no_same_v<typename item<T>::tuple,
		                                 typename item<Ts>::tuple...>) )
		struct protocol;

		template<protocol_setting... Ss, makeing_item_c... Ts>
		    requires(is_protocol_settings_v<protocol_settings<Ss...>>
		             && is_no_same_v<typename item<Ts>::tuple...>)
		struct protocol<protocol_settings<Ss...>, Ts...> {
			using tuple = std::tuple<item<Ts>...>;
			using content_tuple
			    = ::std::tuple<typename item<Ts>::tuple...>;

			using set = make_protocol_setts<Ss...>;

			static constexpr u64 count = ::std::tuple_size_v<tuple>;
			using id_type              = ::xn::smallest_uint_t<count>;

			template<u64 I>
			    requires(I <= count)
			using item = std::tuple_element_t<I, tuple>;
		};

		template<msg_setting... Ss, typename... Is, makeing_item_c... Ts>
		    requires(is_makeing_item_v<settings<Ss...>(Is...)>
		             && is_no_same_v<
		                 typename item<settings<Ss...>(Is...)>::tuple,
		                 typename item<Ts>::tuple...>)
		struct protocol<settings<Ss...>(Is...), Ts...>
		    : public protocol<
		          protocol_settings<protocol_setting::Maximum>,
		          settings<Ss...>(Is...), Ts...> {};

		template<class Pt>
		struct is_protocol: public ::std::false_type {};

		template<typename T, makeing_item_c... Ts>
		struct is_protocol<protocol<T, Ts...>>
		    : public ::std::true_type {};

		XN_USEFUL_MACRO__CONCEPT2(protocol)

		template<protocol_c Pt, tuple_c Tp>
		constexpr typename Pt::id_type refl_item_id() {
			return tuple_index_of_v<typename Pt::content_tuple, Tp>;
		}

		constexpr void copy_add(::std::vector<::std::byte> &target,
		                        u64 &i, auto const value) {
			::std::memcpy(target.data() + i, &value, sizeof(value));
			i += sizeof(value);
		}

		using msg = ::std::vector<::std::byte>;

		template<protocol_c Pt, typename... Ts>
		    requires(refl_item_id<Pt, ::std::tuple<Ts...>>() != Pt::count)
		constexpr ::std::vector<::std::byte> make_msg(Ts &&...vas) {
			constexpr typename Pt::id_type id
			    = refl_item_id<Pt, ::std::tuple<Ts...>>();
			using item = Pt::template item<id>;

			u64 const msg_size = [] {
				if constexpr (Pt::set::is_minumum)
					if constexpr (item::set::is_single)
						return sizeof(id) + item::size;
					else
						return sizeof(id) + sizeof(count_t) + item::size;
				else return sizeof(id) + sizeof(length_t) + item::size;
			}();

			::std::vector<::std::byte> ret(msg_size);
			u64                        i = 0;

			copy_add(ret, i, id);

			if constexpr (Pt::set::is_minumum) {
				if constexpr (!item::set::is_single) {
					count_t count = 1;
					copy_add(ret, i, count);
				}
			} else {
				length_t length = item::size;
				copy_add(ret, i, length);
			}

			if constexpr (item::size > 0)
				(
				    [&] {
					    if constexpr (sizeof(vas) != 0
					                  && !::std::is_empty_v<Ts>) {
						    copy_add(ret, i, vas);
					    }
				    }(),
				    ...);
			return ret;
		}

		template<protocol_c Pt, tuple_c... Tps>
		    requires((refl_item_id<Pt, Tps>()
		              != ::std::tuple_size_v<typename Pt::tuple>)
		             && ...)::std::vector<::std::byte>
		make_packedmsg(Tps &&...vas) {
			using tps_id_type = smallest_uint_t<sizeof...(Tps)>;

			constexpr auto get_id_packs_count
			    = [](u64 i) constexpr -> u64 {
				::std::vector<::std::vector<tps_id_type>> const id_packs
				    = [] constexpr {
					      ::std::vector<
					          ::std::vector<typename Pt::id_type>>
					          ret(Pt::count);
					      loop<::std::tuple<
					          Tps...>>([&]<tps_id_type i, tuple_c Tp> {
						      ret[refl_item_id<Pt, Tp>()].push_back(i);
					      });
					      return ret;
				      }();
				return id_packs[i].size();
			};

			constexpr u64 msg_size = [&] constexpr {
				::std::vector<u64> const part_size = [&] {
					::std::vector<u64> ret(Pt::count);
					loop<Pt::count>([&]<u64 i> {
						using item              = Pt::template item<i>;
						constexpr count_t count = get_id_packs_count(i);
						if constexpr (count == 0) return;

						if constexpr (Pt::set::is_minumum) {
							if constexpr (item::set::is_single)
								ret[i] = sizeof(typename Pt::id_type)
								       + item::size;
							else
								ret[i]
								    = sizeof(typename Pt::id_type)
								    + sizeof(count_t)
								    + count * item::size;
						} else {
							ret[i]
							    = sizeof(typename Pt::id_type)
							    + sizeof(length_t) + count * item::size;
						}
					});
					return ret;
				}();

				u64 ret = 0;
				for (auto const &it : part_size) ret += it;
				return ret;
			}();

			::std::vector<::std::byte> ret(msg_size);
			u64                        i = 0;

			loop<Pt::count>([&]<u64 pt_i> {
				using item              = Pt::template item<pt_i>;
				constexpr count_t count = get_id_packs_count(pt_i);
				if constexpr (count == 0) return;
				constexpr typename Pt::id_type id = pt_i;

				copy_add(ret, i, id);

				if constexpr (Pt::set::is_minumum) {
					if constexpr (item::set::is_single) {
						[&] {
							bool is_seted = false;
							loop<::std::tuple<
							    Tps...>>([&]<tps_id_type iti, tuple_c Tp> {
								if constexpr (refl_item_id<Pt, Tp>()
								              == id) {
									if (is_seted) return;
									is_seted = true;
									auto va  = ::std::get<
                                        iti>(::std::make_tuple(vas...));
									loop<Tp>([&]<u64 itj, typename T> {
										if constexpr (
										    sizeof(T) != 0
										    && !::std::is_empty_v<T>) {
											auto v = ::std::get<itj>(va);
											copy_add(ret, i, v);
										}
									});
								}
							});
						}();
					} else {
						copy_add(ret, i, count);

						[&] {
							loop<::std::tuple<
							    Tps...>>([&]<tps_id_type iti, tuple_c Tp> {
								if constexpr (refl_item_id<Pt, Tp>()
								              == id) {
									auto va = ::std::get<
									    iti>(::std::make_tuple(vas...));
									loop<Tp>([&]<tps_id_type itj,
									             typename T> {
										if constexpr (
										    sizeof(T) != 0
										    && !::std::is_empty_v<T>) {
											auto v = ::std::get<itj>(va);
											copy_add(ret, i, v);
										}
									});
								}
							});
						}();
					}
				} else {
					length_t length = count * (length_t) item::size;
					copy_add(ret, i, length);

					[&] {
						loop<::std::tuple<
						    Tps...>>([&]<tps_id_type iti, tuple_c Tp> {
							if constexpr (refl_item_id<Pt, Tp>() == id) {
								auto va = ::std::get<
								    iti>(::std::make_tuple(vas...));
								loop<Tp>([&]<tps_id_type itj, typename T> {
									if constexpr (
									    sizeof(T) != 0
									    && !::std::is_empty_v<T>) {
										auto v = ::std::get<itj>(va);
										copy_add(ret, i, v);
									}
								});
							}
						});
					}();
				}
			});

			return ret;
		}

		template<protocol_c Pt>
		class MsgPackBuider {
			struct MsgContent {
				count_t                    count = 0;
				::std::vector<::std::byte> content;
			};

			::std::array<MsgContent, Pt::count> msgs;

		  public:
			template<typename... Ts>
			    requires(refl_item_id<Pt, ::std::tuple<Ts...>>()
			             != Pt::count)
			void append(Ts &&...vs) {
				constexpr typename Pt::id_type id
				    = refl_item_id<Pt, ::std::tuple<Ts...>>();
				using item = Pt::template item<id>;

				if constexpr (item::size > 0) {
					::std::vector<::std::byte> ret(item::size);
					u64                        i = 0;

					(
					    [&] {
						    if constexpr (sizeof(vs) != 0
						                  && !::std::is_empty_v<Ts>) {
							    copy_add(ret, i, vs);
						    }
					    }(),
					    ...);

					if constexpr (item::set::is_single) {
						msgs[id].count   = 1;
						msgs[id].content = ::std::move(ret);
					} else {
						msgs[id].count++;
						msgs[id].content.append_range(ret);
					}
				} else {
					if constexpr (item::set::is_single)
						msgs[id].count = 1;
					else msgs[id].count++;
				}
			}

			::std::vector<::std::byte> build() {
				constexpr u64 msg_size = [&] constexpr {
					::std::vector<u64> const part_size = [&] {
						::std::vector<u64> ret(Pt::count);
						loop<Pt::count>([&]<u64 i> {
							using item = Pt::template item<i>;
							constexpr count_t count
							    = get_id_packs_count(i);
							if constexpr (count == 0) return;

							if constexpr (Pt::set::is_minumum) {
								if constexpr (item::set::is_single)
									ret[i]
									    = sizeof(typename Pt::id_type)
									    + item::size;
								else
									ret[i]
									    = sizeof(typename Pt::id_type)
									    + sizeof(count_t)
									    + count * item::size;
							} else {
								ret[i]
								    = sizeof(typename Pt::id_type)
								    + sizeof(length_t)
								    + count * item::size;
							}
						});
						return ret;
					}();

					u64 ret = 0;
					for (auto const &it : part_size) ret += it;
					return ret;
				}();

				::std::vector<::std::byte> ret(msg_size);
			}
		};
	}  //namespace internal

	using set          = internal::msg_setting;
	using protocol_set = internal::protocol_setting;
	template<protocol_set... setting>
	using protocol_sets = internal::protocol_settings<setting...>;
	// 这里的item使用settings而非item，是为了方便外部使用，好用就行，里面我怎么弄得别管，一般人看不懂
	template<set... setting>
	    requires(internal::is_no_resetting<setting...>())
	using item = internal::settings<setting...>;
	/**
	 * @brief
	 * 这个协议完全是编译期协议，只有类型，不用实例化，实例化也啥都没有，当然要是想使用auto黑魔法传递协议，实例化也行，无所谓。
	 *
	 * 推荐使用以下格式使用本协议框架
	 * namespace xn::idl {
	 * enum class user_id:u64{};
	 * class eat{};
	 *
	 * using test_p = protocol<
	 * item<set::Single>(i32, u64), item<set::Multiple>(i32),
	 * item<>(user_id, eat)>;
	 * }
	 *
	 */
	using internal::protocol;

	// 		// template <class P> struct GetHandlerList {
	// 		//   static inline const auto value = []() {
	// 		//     std::array<std::function<void(BufferOnHad)>,
	// 		//     GetMaxId::value - ID + 1>
	// 		//         ret;
	// 		//     ret[0] = [](BufferOnHad buffer) {
	// 		//       if (buffer.size < (sizeof(Ts) + ...))
	// 		//         return;

	// 		//       ProtocolMessageHandler<P, Ts...>::received([&]() {
	// 		//         Ts v;

	// 		//         std::make_integer_sequence<int, 5> memcpy(
	// 		//             &v,
	// 		// buffer.From(tuple_front_i_size<tuple_match_i<Ts,
	// 		//             Ts...>::value,
	// 		// Ts...>::value),
	// 		//             sizeof(Ts));
	// 		//         return v;
	// 		//       }()...);
	// 		//     };
	// 		//     return ret;
	// 		//   }();
	// 		// };
	// 	};

	// 		// template <class P> struct GetHandlerList {
	// 		//   static inline const auto value = []() {
	// 		//     std::array<std::function<void(BufferOnHad)>,
	// 		//     GetMaxId::value - ID + 1>
	// 		//         ret;
	// 		//     auto last = Protocol<ID + 1, Is...>::template
	// 		//     GetHandlerList<P>::value; for (u64 i = 0; i <
	// 		//     last.size(); i++)
	// 		//       ret[i] = last[i];
	// 		//     ret[last.size()] = [](BufferOnHad buffer) {
	// 		//       if (buffer.size < (sizeof(Ts) + ...))
	// 		//         return;

	// 		//       ProtocolMessageHandler<P, Ts...>::received([&]() {
	// 		//         Ts v;

	// 		//         std::make_integer_sequence<int, 5> memcpy(
	// 		//             &v,
	// 		// buffer.From(tuple_front_i_size<tuple_match_i<Ts,
	// 		//             Ts...>::value,
	// 		// Ts...>::value),
	// 		//             sizeof(Ts));
	// 		//         return v;
	// 		//       }()...);
	// 		//     };
	// 		//     return ret;
	// 		//   }();
	// 		// };
}  //namespace idl

}  // namespace xn

#endif
