#pragma once
/**
 * ****************cppgen.hh****************
 * @brief
 * C++代码生成，这是临时的，因为没有做语义分析什么的，只是单纯的把现有的语法树直接按照C++风格输出出来了。
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-24
 * ********************************
 */

/*----------------include----------------*/
#include <fstream>

#include "../3rd/xn_lib/include_me.hh"  // IWYU pragma: keep
#include "../tables/tree_table.hh"

/*----------------body----------------*/
namespace xn::cppgen {
class CppGen {
	using Item = table::TreeTableItem;

  public:
	string const           &output_file_name;
	table::TreeTable const &tree_table;

  private:
	::std::fstream output;

  public:
	CppGen(string const           &output_file_name,
	       table::TreeTable const &tree_table)
	    : output_file_name(output_file_name)
	    , tree_table(tree_table)
	    , output(output_file_name, std::ios::out){};

	void gen();

  private:
	void gen(Item::Command const &node);
	void gen(Item::FnDef const &node);
};

void cpp_gen(string output_file, table::TreeTable const &tree_table);
}
