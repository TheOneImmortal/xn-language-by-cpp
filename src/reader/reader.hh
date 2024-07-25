#pragma once
/**
 * ****************reader.hh****************
 * @brief  词法分析器
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-21
 * ********************************
 */

/*----------------include----------------*/
#include <expected>
#include <istream>

#include "../tables/word_table.hh"
#include "position.hh"

/*----------------body----------------*/
namespace xn::reader {

class Reader {
  private:
	std::istream &input_stream;

	char c = '\0';
	Pos  pos;

	bool is_space     = false;
	bool is_newline   = true;
	bool is_in_string = false;

  public:
	Reader(::std::istream &stream);
	~Reader();

	::std::vector<table::WordTableItem> read();

  private:
	void read_number(::std::vector<table::WordTableItem> &items);
	void read_symbol(::std::vector<table::WordTableItem> &items);
	void read_command(::std::vector<table::WordTableItem> &items);
	void read_name(::std::vector<table::WordTableItem> &items);

	/**
	 * @brief 跳过空白部分，可能不会移动
	 */
	void skip_whitespace(::std::vector<table::WordTableItem> &);

	void pop_char();

	bool is_valid_for_identifier(char c);
};

table::WordTable file_read(::std::string const &filename);

::std::vector<table::WordTableItem> read(
    ::std::string_view const input);

}  //namespace xn::reader
