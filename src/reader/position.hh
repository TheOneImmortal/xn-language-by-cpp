#pragma once
/**
 * ****************position.hh****************
 * @brief  位置记录
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-21
 * ********************************
 */

/*----------------include----------------*/
#include <cstdint>
#include <format>
#include <string>

/*----------------body----------------*/
namespace xn::reader {

struct Pos {
	int     line = 0, col = 0;
	int64_t count = 0;

	void inc(bool new_line) {
		count++;
		if (new_line) {
			line++;
			col = 0;
			return;
		}
		col++;
	}

	operator ::std::string() const {
		return ::std::format("{}:{}", line + 1, col + 1);
	}
};

class PosRange {
  public:
	Pos start, end;

  public:
	PosRange() {}

	PosRange(Pos start): start(start), end(start) {}

	PosRange(Pos start, Pos end): start(start), end(end) {}

	operator ::std::string() const {
		return ::std::format("{} - {}", ::std::string(start),
		                     ::std::string(end));
	}
};

}  //namespace xn::reader
