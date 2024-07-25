/**
 * ****************byte_operation.hh****************
 * @brief  对字节进行操作
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__BYTE_OPERATION_HH__
#define __XN_BT__BYTE_OPERATION_HH__

/*----------------include----------------*/
#include "./base_type.hh"

/*----------------body----------------*/
namespace xn {

constexpr u16 u16_by_u8(int_c val high, int_c val low) noexcept {
	return u16(((u16) (high & 0xff) << 8) | (u16) (low & 0xff));
}

constexpr u32 u32_by_u16(int_c val high, int_c val low) noexcept {
	return u32(((u32) (high & 0xffff) << 16) | (u32) (low & 0xffff));
}

constexpr u64 u64_by_u32(int_c val high, int_c val low) noexcept {
	return u64(((u64) (high & 0xff'ff'ff'ff) << 32)
	           | (u64) (low & 0xff'ff'ff'ff));
}

constexpr u8 u8h_in_u16(int_c val u16) noexcept {
	return (u8) (u16 >> 8);
}

constexpr u8 u8l_in_u16(int_c val u16) noexcept { return (u8) u16; }

constexpr u16 u16h_in_u32(int_c val u32) noexcept {
	return (u16) (u32 >> 16);
}

constexpr u16 u16l_in_u32(int_c val u32) noexcept { return (u16) u32; }

constexpr u32 u32h_in_u64(int_c val u64) noexcept {
	return (u32) (u64 >> 32);
}

constexpr u32 u32l_in_u64(int_c val u64) noexcept { return (u32) u64; }

constexpr u16 u16_by_u8(u8 const high, u8 const low) noexcept {
	return u16(((u16) high << 8) | (u16) low);
}

constexpr u32 u32_by_u16(u16 const high, u16 const low) noexcept {
	return u32(((u32) high << 16) | (u32) low);
}

constexpr u64 u64_by_u32(u32 const high, u32 const low) noexcept {
	return u64(((u64) high << 32) | (u64) low);
}

}  // namespace xn

#endif
