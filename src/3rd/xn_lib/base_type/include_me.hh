/**
 * ****************include_me.hh****************
 * @brief  基础类型
 * @author https://github.com/TheOneImmortal
 * @date   2024-01-16
 * ********************************
 */

#ifndef __XN_BT__INCLUDE_ME_HH__
#define __XN_BT__INCLUDE_ME_HH__

/*----------------include----------------*/
#include "src/base_type.hh"              // IWYU pragma: export
#include "src/byte_operation.hh"         // IWYU pragma: export
#include "src/cencept_extension.hh"      // IWYU pragma: export
#include "src/format_adaptation.hh"      // IWYU pragma: export
#include "src/string_extension.hh"       // IWYU pragma: export
#include "src/type_conversion.hh"        // IWYU pragma: export
#include "src/type_traits_extension.hh"  // IWYU pragma: export

/*----------------body----------------*/

using xn::operator""_f;
using xn::operator""_fln;
using xn::operator""_out;
using xn::operator""_outln;

#endif
