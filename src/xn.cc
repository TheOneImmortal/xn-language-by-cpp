#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include <string>

#include "log/log.hh"
#include "reader/reader.hh"
#include "tmp-cppcode_gen/cppgen.hh"
#include "tree/tree.hh"

int main(int argc, char const *argv[]) {
	::std::cin.imbue(::std::locale("chs"));
	::std::cout.imbue(::std::locale("chs"));

	::std::string         file_name;
	std::filesystem::path dir;

	if (argc <= 1) {
		::std::println(::std::cout, "ERROR, 文件呢？");
		system("pause");
		return -1;
	} else {
		file_name = argv[1];
		dir       = ::std::filesystem::path(file_name).parent_path();
		xn::log::log_file.open(dir / "debug.log", ::std::ios::out);
		xn::log::error_file.open(dir / "error.log", ::std::ios::out);
	}
	::xn::log::important_log("file_name: {}", file_name);

	::xn::log::important_log(
	    "--------------------build words--------------------");

	auto word_table = xn::reader::file_read(file_name);

	::xn::log::important_log(
	    "--------------------build words result--------------------");

	for (auto const &item : word_table.items)
		::xn::log::debug_log("{}", ::std::string(item));

	::xn::log::important_log(
	    "--------------------build tree--------------------");

	auto tree_table = xn::tree::build_tree(word_table);

	::xn::log::important_log(
	    "--------------------build tree result--------------------");

	for (auto const &item : tree_table.items)
		::xn::log::debug_log("{}", ::std::string(item));

	::xn::log::important_log(
	    "--------------------cpp gen--------------------");

	xn::cppgen::cpp_gen(dir / "output.cc", tree_table);

	::xn::log::important_log(
	    "--------------------over--------------------");
	system("pause");
}
