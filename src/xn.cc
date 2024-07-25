#include <format>
#include <iostream>
#include <print>
#include <string>

#include "reader/reader.hh"
#include "tmp-cppcode_gen/cppgen.hh"
#include "tree/tree.hh"

int main(int argc, char const *argv[]) {
	::std::cin.imbue(::std::locale("chs"));
	::std::cout.imbue(::std::locale("chs"));

	::std::string file_name;

	if (argc <= 1) {
		::std::println(::std::cout, "ERROR, 文件呢？");
		system("pause");
		return -1;
	} else {
		file_name = argv[1];
	}
	::std::println(::std::cout, "file_name: {}", file_name);

	::std::println(
	    ::std::cout,
	    "--------------------build words--------------------");

	auto word_table = xn::reader::file_read(file_name);

	::std::println(
	    ::std::cout,
	    "--------------------build words result--------------------");

	for (auto const &item : word_table.items)
		::std::println("{}", ::std::string(item));

	::std::println(
	    ::std::cout,
	    "--------------------build tree--------------------");

	auto tree_table = xn::tree::build_tree(word_table);

	::std::println(
	    ::std::cout,
	    "--------------------build tree result--------------------");

	for (auto const &item : tree_table.items)
		::std::println("{}", ::std::string(item));

	::std::println(::std::cout,
	               "--------------------cpp gen--------------------");

	xn::cppgen::cpp_gen("output.cc", tree_table);

	::std::println(::std::cout,
	               "--------------------over--------------------");
}
