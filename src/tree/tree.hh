#pragma once
/**
 * ****************tree.hh****************
 * @brief  语法树构建
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-21
 * ********************************
 */

/*----------------include----------------*/
#include <stack>

#include "../3rd/xn_lib/include_me.hh"  // IWYU pragma: keep
#include "../tables/tree_table.hh"
#include "../tables/word_table.hh"

/*----------------body----------------*/
namespace xn::tree {

class Tree {
	using WordItem = table::WordTableItem;
	using Words    = ::std::vector<WordItem>;
	using TreeItem = table::TreeTableItem;
	using Table    = table::TreeTable;

	Words const      &words;
	i32               i;
	::std::stack<i32> levels;

  public:
	Tree(table::WordTable const &words): words(words.items), i(0) {
		levels.push(0);
	}

	Table build();

  private:
	void try_skip_whitespace(bool const is_skip_line);
	bool try_skip_symbol(bool const             is_skip_line,
	                     WordItem::Symbol const expect);
	void just_skip_line();
	bool just_skip_symbol(bool const             is_skip_line,
	                      WordItem::Symbol const expect);
	template<typename ExpectT>
	bool try_skip_word(bool const is_skip_line, auto &&f)
	    requires(
	        ::std::is_invocable_r_v<bool, decltype(f), ExpectT const &>)
	;
	template<typename ExpectT>
	bool just_skip_word(bool const is_skip_line, auto &&f)
	    requires(
	        ::std::is_invocable_r_v<bool, decltype(f), ExpectT const &>)
	;

	::std::optional<TreeItem::Name>      try_build_name();
	::std::optional<TreeItem::CapString> try_build_捕获字符串();
	::std::optional<TreeItem::Exp>       try_build_表达式();
	::std::optional<TreeItem::Exp>       try_build_表达式(
	          TreeItem::Name &&name);
	::std::optional<TreeItem::Type>   try_build_类型();
	::std::optional<TreeItem::VarDef> try_build_变量定义();
	::std::optional<TreeItem::VarDef> try_build_变量定义(
	    TreeItem::Name &&name);
	::std::unique_ptr<TreeItem::ProcessStream> try_build_else__表达式_x(
	    TreeItem::Exp &&exp);
	::std::unique_ptr<TreeItem::ProcessStream> try_build_else流();
	::std::optional<TreeItem::IfStream> try_build_条件流控制();
	::std::optional<TreeItem::IfStream> try_build_条件流控制(
	    TreeItem::Exp &&cond);
	::std::optional<TreeItem::WhileStream> try_build_while循环流控制();
	::std::optional<TreeItem::ForStream> try_build_for循环流控制();
	::std::optional<TreeItem::Process>   try_build_语句__表达式_x(
	      TreeItem::Exp &&exp);
	::std::optional<TreeItem::Process> try_build_语句();
	::std::optional<TreeItem::Process> try_build_语句(
	    TreeItem::Name &&name);

	void build_仙言(Table &table);
	void build_命令(WordItem::Symbol const &now, Table &table);
	void build_函数定义(Table &table);
};

table::TreeTable build_tree(table::WordTable const &words);

}  //namespace xn::tree
