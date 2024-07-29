#pragma once
/**
 * ****************type_table.hh****************
 * @brief  类型表
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-26
 * ********************************
 */

/*----------------include----------------*/
#include <string>
#include <variant>
#include <vector>

#include "../3rd/xn_lib/include_me.hh"  // IWYU pragma: keep

/*----------------body----------------*/
namespace xn::table {

struct TypeTable {
	struct TemplateType;
	struct SurfaceType;

	struct SingleType {
		::std::vector<::std::variant<::std::string, TemplateType>>
		    value;
	};

	struct TemplateType {
		::std::string                name;
		::std::vector<::std::string> templates;
	};

	struct LinkType {
		::std::vector<i32> value;
	};

	struct UnionType {
		::std::vector<i32> value;
	};

	struct PointerType {
		i32 value;
	};

	struct SurfaceType {
		bool is_mut;
		bool is_ref;

		::std::variant<SingleType, LinkType, UnionType> value;
	};

	::std::vector<SurfaceType> types;
};

}  //namespace xn::table
