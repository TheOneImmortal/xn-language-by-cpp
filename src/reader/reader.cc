#include "reader.hh"

#include <cstdio>
#include <cstring>
#include <expected>
#include <fstream>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

#include "../3rd/xn_lib/include_me.hh"  // IWYU pragma: keep
#include "../log/log.hh"

bool is_whitespace(char c) { return strchr(" \n\t\r", c); }

bool is_symbol_char(char c) {
	return strchr("#+-*/%|&^~!<>=(){}[]:;,\'.?\"`$\\@", c);
}

bool is_number(char c) { return c >= '0' && c <= '9'; }

namespace xn::reader {

using Item  = table::WordTableItem;
using Items = ::std::vector<Item>;

Reader::Reader(::std::istream &stream)
    : input_stream(stream)
    , c(input_stream.get()) {}

Reader::~Reader() { log::debug_log("Destroying the reader"); }

::std::vector<Item> Reader::read() {
	log::debug_log("Read start read");

	Items table_items{};

	skip_whitespace(table_items);

	while (c != EOF) {
		if (is_symbol_char(c)) read_symbol(table_items);
		else if (is_number(c)) read_number(table_items);
		else read_name(table_items);

		skip_whitespace(table_items);
	}
	return table_items;
}

void Reader::read_command(Items &items) {
	Item cmdd = {
		.position_range = { pos, pos },
		.value          = Item::Symbol{ Item::Symbol::Cmdd }
	};

	pop_char();  // skip '#'

	Item::Name name;
	Item       item{};
	item.position_range.start = pos;

	while (c != EOF && c != '\n') {
		if (c == '#') {  // 里面就是注释了
			while (c != EOF && c != '\n') pop_char();
			break;
		}
		name.value += c;
		pop_char();
	}

	if (name.value.empty()) {
		if (!items.empty()) {
			Item             &last = items.at(items.size() - 1);
			Item::Whitespace *p
			    = std::get_if<Item::Whitespace>(&last.value);

			if (p) {
				last.position_range.end = pos;
				return;
			}
		}
		item.position_range.end = pos;
		item.value.emplace<Item::Whitespace>(Item::Whitespace::Space);
		items.push_back(::std::move(item));
		return;
	}

	items.push_back(cmdd);

	item.value              = name;
	item.position_range.end = pos;
	items.push_back(::std::move(item));
}

void Reader::read_name(Items &items) {
	Item::Name name;
	Item       item{};
	item.position_range.start = pos;
	do {
		name.value += c;
		pop_char();
	} while (c != EOF && !is_whitespace(c) && !is_symbol_char(c));

	auto pair   = table::internal::str_keyword_map.find(name.value);
	auto is_key = pair != table::internal::str_keyword_map.end();
	if (is_key) item.value = pair->second;
	else {
		if (name.value.contains("__"))
			log::error_log(
			    "\"__\" may be collision with rename cpp actual name!");
		item.value = ::std::move(name);
	}

	item.position_range.end = pos;

	items.push_back(::std::move(item));
}

#define UPDATE_symbol_WITH_SYMBOL_TYPE(SYMBOL_TYPE) \
	symbol.position_range.end = pos;                \
	symbol.value.emplace<Item::Symbol>(Item::Symbol::SYMBOL_TYPE);

#define READ_SYMBOL__SYMBOL_EQ(SYMBOL, NEQ_TYPE, EQ_TYPE)        \
	Item symbol = {                                              \
		.position_range = { pos, pos },                          \
		.value          = Item::Symbol{ Item::Symbol::NEQ_TYPE } \
	};                                                           \
	pop_char();                                                  \
	if (c != '=') items.push_back(symbol);                       \
	else {                                                       \
		UPDATE_symbol_WITH_SYMBOL_TYPE(EQ_TYPE);                 \
		pop_char();                                              \
		items.push_back(symbol);                                 \
	}

#define READ_SYMBOL_CASE__SYMBOL_EQ(SYMBOL, NEQ_TYPE, EQ_TYPE) \
	case SYMBOL: {                                             \
		READ_SYMBOL__SYMBOL_EQ(SYMBOL, NEQ_TYPE, EQ_TYPE)      \
	} break

#define READ_SYMBOL_CASE__SYMBOL(SYMBOL, TYPE)                   \
	case SYMBOL: {                                               \
		Item symbol = {                                          \
			.position_range = { pos, pos },                      \
			.value          = Item::Symbol{ Item::Symbol::TYPE } \
		};                                                       \
		pop_char();                                              \
		items.push_back(symbol);                                 \
	} break

void Reader::read_number(Items &items) {
	u64  value  = 0;
	Item number = {
		.position_range = {pos, pos},
	};
	while (is_number(c)) {
		value                     = value * 10 + (c - '0');
		number.position_range.end = pos;
		pop_char();
	}
	if (c == '.') {
		if (input_stream.peek() == '.') {
			number.value = Item::Number{ value };
			items.push_back(number);
			return;
		}

		bool is_float = true;
		visit__start();
		if_T_is(Item::Symbol) {
			if (item == Item::Symbol::RGet) is_float = false;
		}
		visit__end((items.end() - 1)->value);
		if (is_float) {
			pop_char();
			f64 value   = value;
			u8  decimal = 0;
			while (is_number(c)) {
				value += (c - '0') / pow(10.f, ++decimal);
				pop_char();
			}
			number.position_range.end = pos;
			number.value              = Item::Number{ value };
			items.push_back(number);
			return;
		}
	}
	number.position_range.end = pos;
	number.value              = Item::Number{ value };
	items.push_back(number);
}

void Reader::read_symbol(Items &items) {
	switch (c) {
	case '#': {
		read_command(items);
	} break;

	case '+': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item ::Symbol{ Item ::Symbol ::Plus }
		};
		pop_char();
		if (c == '=') {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item ::Symbol>(Item ::Symbol ::PluE);

			pop_char();
			items.push_back(symbol);
		} else if (c == '+') {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item ::Symbol>(Item::Symbol::PluS);

			pop_char();
			items.push_back(symbol);
		} else items.push_back(symbol);
	} break;
	case '-': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item ::Symbol{ Item ::Symbol ::Minu }
		};
		pop_char();
		if (c == '=') {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item ::Symbol>(Item ::Symbol ::MinE);

			pop_char();
			items.push_back(symbol);
		} else if (c == '-') {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item ::Symbol>(Item ::Symbol ::MinS);

			pop_char();
			items.push_back(symbol);
		} else items.push_back(symbol);
	} break;
		READ_SYMBOL_CASE__SYMBOL_EQ('*', Muti, MutE);
		READ_SYMBOL_CASE__SYMBOL_EQ('/', Divd, DivE);
		READ_SYMBOL_CASE__SYMBOL_EQ('%', Modu, ModE);
		READ_SYMBOL_CASE__SYMBOL_EQ('|', Orrr, OrrE);
		READ_SYMBOL_CASE__SYMBOL_EQ('&', Andd, AndE);
		READ_SYMBOL_CASE__SYMBOL_EQ('^', Xorr, XorE);
		READ_SYMBOL_CASE__SYMBOL_EQ('~', Oppo, OppE);
		READ_SYMBOL_CASE__SYMBOL_EQ('!', Nott, NotE);
		READ_SYMBOL_CASE__SYMBOL_EQ('=', Equl, Same);
		READ_SYMBOL_CASE__SYMBOL_EQ(':', DfTp, DfAt);

		READ_SYMBOL_CASE__SYMBOL('(', ExpL);
		READ_SYMBOL_CASE__SYMBOL(')', ExpR);
		READ_SYMBOL_CASE__SYMBOL('{', SwiL);
	case '}': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item ::Symbol{ Item ::Symbol ::SwiR }
		};
		pop_char();
		items.push_back(symbol);

		// FIXME:
		// 这种处理字符串捕获的方法使的捕获不能嵌套，而且不能有内部的花括号
		if (is_in_string) {
			Item content = {
				.position_range = {pos, pos},
			};

			Item::String content_text;
			while (c != '`' && c != EOF && c != '{') {
				content.position_range.end = pos;
				content_text.value        += c;
				pop_char();
			}
			content.value.emplace<Item::String>(
			    ::std::move(content_text));

			if (c == '{') {
				pop_char();
				items.push_back(content);
				break;
			} else if (c != '`') {
				log::error_log("正在读取捕获字符串, unexpected EOF");
				items.push_back(content);
				break;
			}

			is_in_string = false;
			pop_char();
			items.push_back(content);

			Item symbol = {
				.position_range = { pos, pos },
				.value          = Item::Symbol{ Item::Symbol::CapS }
			};
			items.push_back(symbol);
		}
	} break;
		READ_SYMBOL_CASE__SYMBOL('[', ArrL);
		READ_SYMBOL_CASE__SYMBOL(']', ArrR);
		READ_SYMBOL_CASE__SYMBOL(';', Endd);
		READ_SYMBOL_CASE__SYMBOL(',', Coma);
		READ_SYMBOL_CASE__SYMBOL('\'', CGet);
		READ_SYMBOL_CASE__SYMBOL('?', Mayy);
		READ_SYMBOL_CASE__SYMBOL('$', Poin);
		READ_SYMBOL_CASE__SYMBOL('\\', Bslh);
		READ_SYMBOL_CASE__SYMBOL('@', Attt);

	case '<': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item::Symbol{ Item::Symbol::Less }
		};
		pop_char();
		switch (c) {
		case '=': {
			UPDATE_symbol_WITH_SYMBOL_TYPE(LeEq);
			pop_char();
			items.push_back(symbol);
		} break;

		case '<': {
			UPDATE_symbol_WITH_SYMBOL_TYPE(LMov);
			pop_char();
			if (c != '=') items.push_back(symbol);
			else {
				UPDATE_symbol_WITH_SYMBOL_TYPE(LMoE);
				items.push_back(symbol);
			}
		} break;

		default: items.push_back(symbol);
		}
	} break;

	case '>': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item::Symbol{ Item::Symbol::Gret }
		};
		pop_char();
		switch (c) {
		case '=': {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item::Symbol>(Item::Symbol::GrEq);
			pop_char();
			items.push_back(symbol);
		} break;

		case '>': {
			symbol.position_range.end = pos;
			symbol.value.emplace<Item::Symbol>(Item::Symbol::RMov);
			pop_char();
			if (c != '=') items.push_back(symbol);
			else {
				symbol.position_range.end = pos;
				symbol.value.emplace<Item::Symbol>(Item::Symbol::RMoE);
				pop_char();
				items.push_back(symbol);
			}
		} break;

		default: items.push_back(symbol);
		}
	} break;

	case '.': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item::Symbol{ Item::Symbol::RGet }
		};
		pop_char();
		if (c != '.') {
			items.push_back(symbol);
			break;
		}
		symbol.position_range.end = pos;
		symbol.value.emplace<Item::Symbol>(Item::Symbol::Rang);
		pop_char();
		if (c != '.') {
			items.push_back(symbol);
			break;
		}
		symbol.position_range.end = pos;
		symbol.value.emplace<Item::Symbol>(Item::Symbol::Many);
		pop_char();
		items.push_back(symbol);
	} break;

	case '"': {
		Item content = {
			.position_range = {pos, pos},
		};
		pop_char();

		Item::String content_text;
		while (c != '"' && c != EOF) {
			content.position_range.end = pos;
			content_text.value        += c;
			pop_char();
		}
		content.value.emplace<Item::String>(::std::move(content_text));

		if (c != '"') {
			log::error_log("正在读取字符串, unexpected EOF");
			items.push_back(content);
			break;
		}

		content.position_range.end = pos;
		pop_char();
		items.push_back(content);
	} break;
	case '`': {
		Item symbol = {
			.position_range = { pos, pos },
			.value          = Item::Symbol{ Item::Symbol::CapS }
		};
		items.push_back(symbol);
		pop_char();
		if (is_in_string) {
			is_in_string = false;
			break;
		}

		Item content = {
			.position_range = {pos, pos},
		};

		Item::String content_text;
		while (c != '`' && c != EOF && c != '{') {
			content.position_range.end = pos;
			content_text.value        += c;
			pop_char();
		}
		content.value.emplace<Item::String>(::std::move(content_text));

		if (c == '{') {
			pop_char();
			items.push_back(content);

			Item symbol = {
				.position_range = { pos, pos },
				.value          = Item::Symbol{ Item::Symbol::SwiL }
			};
			items.push_back(symbol);

			is_in_string = true;
			break;
		} else if (c != '`') {
			log::error_log("正在读取捕获字符串, unexpected EOF");
			items.push_back(content);
			break;
		}

		pop_char();
		items.push_back(content);
	} break;

	default: {
		log::error_log("unknown symbol: {}", c);
	}
	}
}

#define SKIP_WHITESPACE__SKIPING()                      \
	while (is_whitespace(c)) {                          \
		if (!is_newline && (c == '\n' || c == '\r')) {  \
			is_newline = true;                          \
			whitespace.value.emplace<Item::Whitespace>( \
			    Item::Whitespace::Newline);             \
		}                                               \
		whitespace.position_range.end = pos;            \
		pop_char();                                     \
	}

void Reader::skip_whitespace(Items &items) {
	if (!is_whitespace(c)) return;

	if (!items.empty()) {
		Item             &last = items.at(items.size() - 1);
		Item::Whitespace *p
		    = std::get_if<Item::Whitespace>(&last.value);

		if (p) {
			Item &whitespace = last;
			bool  is_newline = (*p == Item::Whitespace::Newline);
			SKIP_WHITESPACE__SKIPING();
			return;
		}
	}

	Item whitespace = {
		.position_range = { pos, pos },
		.value          = Item::Whitespace{ Item::Whitespace::Space }
	};
	bool is_newline = false;
	SKIP_WHITESPACE__SKIPING();
	items.push_back(whitespace);
}

void Reader::pop_char() {
	pos.inc(c == '\n');

	c = input_stream.get();
}

bool Reader::is_valid_for_identifier(char const c) {
	switch (c) {
	case '!':
	case '$':
	case '%':
	case '&':
	case '*':
	case '+':
	case '-':
	case '.':
	case '~':
	case '/':
	case ':':
	case '<':
	case '=':
	case '>':
	case '?':
	case '@':
	case '^':
	case '_': return true;
	}

	if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z'
	    || '0' <= c && c <= '9')
		return true;

	return false;
}

table::WordTable file_read(::std::string const &filename) {
	log::debug_log("start file read");
	table::WordTable word_table;
	word_table.filename = filename;

	::std::ifstream file(filename);
	if (!file.is_open()) {
		log::error_log("file [{}] open failed.", filename.c_str());
		return word_table;
	}

	Reader reader(file);
	word_table.items = reader.read();

	return word_table;
}

::std::vector<table::WordTableItem> read(
    ::std::string_view const input) {
	::std::istringstream iss(input.data());

	Reader reader(iss);
	return reader.read();
}
}  //namespace xn::reader
