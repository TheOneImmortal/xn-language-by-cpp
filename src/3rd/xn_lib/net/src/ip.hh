/**
 * ****************ip.hh****************
 * @brief  将常用IP提前计算为二进制
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_BN__IP_HH__
#define __XN_BN__IP_HH__

/*----------------include----------------*/
#pragma region include

#include <winsock2.h>

#include "../../base_type/include_me.hh"

#pragma endregion

/*----------------body----------------*/
namespace xn {

// 0.0.0.0 无限定IP
constexpr u64 ip_any = 0x00'00'00'00;
// 127.0.0.1 本机回环IP
constexpr u64 ip_local = 0x7f'00'00'01;

// 0.0.0.0 无限定IP——网络序
u64 const net_ip_any = htonl(ip_any);
// 127.0.0.1 本机回环IP——网络序
u64 const net_ip_local = htonl(ip_local);

}  // namespace xn

#endif
