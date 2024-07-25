/**
 * ****************message_handler.hh****************
 * @brief  协议的辅助，消息收发接口
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-10
 * ********************************
 */

#ifndef __XN_ILP__MESSAGE_HANDLER_HH__
#define __XN_ILP__MESSAGE_HANDLER_HH__

/*----------------include----------------*/
#include <optional>
#include <tuple>
#include <utility>

#include "../../base_type/include_me.hh"
#include "../../easy_template/include_me.hh"
#include "protocol.hh"

/*----------------body----------------*/
/*
namespace xn {

// template <u64 id, class Tpl> class call_helper;
// template <u64 id, class... Ts> class call_helper<id,
// std::tuple<Ts...>> { public:
//   template <u64... Is>
//   void operator()(auto &f, BufferOnHad data,
//                   std::index_sequence<Is...>) const noexcept {
//     f.template operator()([&data]() -> Ts {
//       return from_bytes<Ts>(data.From(
//           tuple_front_i_size<Is, std::tuple<Ts...>>::value,
//           sizeof(Ts)));
//     }()...);
//   };
//   void operator()(auto &f, BufferOnHad data) const noexcept {
//     operator()(f, data, std::make_index_sequence<sizeof...(Ts)>());
//   };
// };
namespace idl_protocol {

#pragma region exports

    template<class P>
    class Handler;

    template<class Handler>
    void parser(BufferOnHad data);
    template<class Handler>
    BufferOnHeap serialize(auto &&...var);

#pragma endregion

#pragma region realization

    namespace helper {
        template<class H, class Tpl>
        class call_helper;

        template<class H, class... Ts>
        class call_helper<H, std::tuple<Ts...>> {
          public:
            template<u64... Is>
            void operator()(BufferOnHad data,
std::index_sequence<Is...>) const noexcept { H::recived([&data]() -> Ts
{ return from_bytes<Ts>(data.From( tuple_front_i_size<Is,
std::tuple<Ts...>>::value, sizeof(Ts)));
                }()...);
            }

            void operator()(BufferOnHad data) const noexcept {
                operator()(data,
                           std::make_index_sequence<sizeof...(Ts)>());
            }
        };

        struct breaked {
            const ID          id;
            u16 const         length;
            BufferOnHad const content;
            BufferOnHad const next;
        };

        std::optional<breaked> get_break(BufferOnHad data) {
            if ((sizeof(ID) + sizeof(u16)) > data.size)
                return std::nullopt;

            auto id = from_bytes<ID>(data.From(0, sizeof(ID)));
            auto length
                = from_bytes<u16>(data.From(sizeof(ID), sizeof(u16)));
            auto content = data.From(sizeof(ID) + sizeof(u16), length);
            if ((sizeof(ID) + sizeof(u16) + length) > data.size)
                return std::nullopt;
            auto next = data.From(sizeof(ID) + sizeof(u16) + length);

            return breaked{
                .id      = id,
                .length  = length,
                .content = content,
                .next    = next,
            };
        }

        template<class H>
        class Parser;

        template<template<class P> class H, class P>
        class Parser<H<P>> {
          public:
            Parser() noexcept {
                loop<typename P::tuple>([this]<u64 i, typename T>() {
                    parsers[i] = [](BufferOnHad data) {
                        if (data.size > P::template size<i>) {
                            call_helper<H<P>, typename P::template item<
                                                  i>::tuple>()(data);
                        }
                    };
                });
            }

            void operator()(BufferOnHad data) const {
                std::cout << "parser" << std::endl;
                while (1) {
                    auto broken_opt = get_break(data);

                    if (!broken_opt.has_value()) break;

                    auto broken = broken_opt.value();

                    if (broken.id < parsers.size())
                        parsers[broken.id](broken.content);
                    else
                        std::cout
                            << "unknown id:" << broken.id << std::endl;

                    data = std::move(broken.next);
                }
            }

          private:
            std::array<std::function<void(BufferOnHad)>, P::count>
                parsers;
        };

        template<class Protocol>
        class Serialize;

        template<class... Is>
        class Serialize<idl_protocol::Protocol<Is...>> {
            using P = idl_protocol::Protocol<Is...>;

          public:
            Serialize() noexcept {}

            BufferOnHeap operator()(auto const &...var) const {
                using item = idl_protocol::Item<
                    std::remove_cvref_t<decltype(var)>...>;

                BufferOnHeap
                    buffer(sizeof(ID) + sizeof(u16) + item::size);
                to_bytes(buffer,
                         ID(tuple_match_i<item, typename
P::tuple>::value), (u16) item::size, var...);

                return buffer;
            }
        };
    }  // namespace helper

    template<class Handler>
    inline void parser(BufferOnHad data) {
        static helper::Parser<Handler> parser;

        parser(data);
    }

    template<class Protocol>
    inline BufferOnHeap serialize(auto &&...var) {
        static helper::Serialize<Protocol> serialize;

        return serialize(decltype(var)(var)...);
    }

#pragma endregion

}  // namespace idl_protocol

}  // namespace xn
*/
#endif
