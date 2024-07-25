#pragma once
/**
 * ****************word_table.hh****************
 * @brief  词法分析表
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-21
 * ********************************
 */

/*----------------include----------------*/
#include <cstddef>
#include <format>
#include <map>
#include <print>
#include <string>
#include <variant>
#include <vector>

#include "../3rd/xn_lib/include_me.hh"  // IWYU pragma: keep
#include "../reader/position.hh"
#include "../useful_macro.hh"

/*----------------body----------------*/
namespace xn::table {

struct WordTableItem {
	enum class Whitespace { Space, Newline };

	enum class Symbol {
		Cmdd,  // #

		Plus,  // +
		Minu,  // -
		Muti,  // *
		Divd,  // /
		Powr,  // **
		RoOp,  // //
		Modu,  // %
		Orrr,  // |
		Andd,  // &
		Xorr,  // ^
		Oppo,  // ~
		Nott,  // !
		LMov,  // <<
		RMov,  // >>

		PluE,  // +=
		MinE,  // -=
		MutE,  // *=
		DivE,  // /=
		ModE,  // %=
		OrrE,  // |=
		AndE,  // &=
		XorE,  // ^=
		OppE,  // ~=
		NotE,  // !=
		LMoE,  // <<=
		RMoE,  // >>=

		Equl,  // =
		Same,  // ==
		Less,  // <
		Gret,  // >
		LeEq,  // <=
		GrEq,  // >=

		PluS,  // ++
		MinS,  // --

		ExpL,  // (
		ExpR,  // )
		SwiL,  // {
		SwiR,  // }
		ArrL,  // [
		ArrR,  // ]
		DfTp,  // :
		Endd,  // ;

		DfAt,  // :=

		Coma,  // ,

		CGet,  // '
		RGet,  // .

		Mayy,  // ?

		Stri,  // "
		       // 虽然分配了这个符号的枚举，但是由于会被直接处理成字符串，表中不记录本符号，所以实际不会使用
		CapS,  // `

		Poin,  // $

		Bslh,  // '\'

		Rang,  // ..
		Many,  // ...

		Attt,  // @
	};

	enum class Keyword {
		If,
		El,
		Elif,

		While,
		For,
		Step,
		Break,

		Case,

		Vr,
		Fn,
		Mc,

		Inline,

		With,
		As,
	};

	struct Name {
		::std::string value;
	};

	struct Unknow {
		::std::string text;
	};

	struct Number: public ::std::variant<u64, f64> {
		operator ::std::string() const {
			::std::string s = "";
			visit__start();
			if_T_is(u64) s   += "vr i64: " + ::std::to_string(item);
			elif_T_is(f64) s += "vr f64: " + ::std::to_string(item);
			else s           += "unknown"; visit__end(*this);
			return s;
		}
	};

	struct String {
		::std::string value;
	};

	using ItemType
	    = ::std::variant<nullptr_t, Whitespace, Symbol, Keyword, Name,
	                     Unknow, Number, String>;

	reader::PosRange position_range;
	ItemType         value;

	operator ::std::string() const;
};

struct WordTable {
	::std::string                filename;
	::std::vector<WordTableItem> items;
};

namespace internal {

	using Symbol  = WordTableItem::Symbol;
	using Keyword = WordTableItem::Keyword;

	inline ::std::map<Symbol const, ::std::string const> const
	    symbol_str_map{
		    {Symbol::Cmdd,   "#"},
            {Symbol::Plus,   "+"},
		    {Symbol::Minu,   "-"},
            {Symbol::Muti,   "*"},
		    {Symbol::Divd,   "/"},
            {Symbol::Powr,  "**"},
		    {Symbol::RoOp,  "//"},
            {Symbol::Modu,   "%"},
		    {Symbol::Orrr,   "|"},
            {Symbol::Andd,   "&"},
		    {Symbol::Xorr,   "^"},
            {Symbol::Oppo,   "~"},
		    {Symbol::Nott,   "!"},
            {Symbol::LMov,  "<<"},
		    {Symbol::RMov,  ">>"},
            {Symbol::PluE,  "+="},
		    {Symbol::MinE,  "-="},
            {Symbol::MutE,  "*="},
		    {Symbol::DivE,  "/="},
            {Symbol::ModE,  "%="},
		    {Symbol::OrrE,  "|="},
            {Symbol::AndE,  "&="},
		    {Symbol::XorE,  "^="},
            {Symbol::OppE,  "~="},
		    {Symbol::NotE,  "!="},
            {Symbol::LMoE, "<<="},
		    {Symbol::RMoE, ">>="},
            {Symbol::Equl,   "="},
		    {Symbol::Same,  "=="},
            {Symbol::Less,   "<"},
		    {Symbol::Gret,   ">"},
            {Symbol::LeEq,  "<="},
		    {Symbol::GrEq,  ">="},
            {Symbol::PluS,  "++"},
		    {Symbol::MinS,  "--"},
            {Symbol::ExpL,   "("},
		    {Symbol::ExpR,   ")"},
            {Symbol::SwiL,   "{"},
		    {Symbol::SwiR,   "}"},
            {Symbol::ArrL,   "["},
		    {Symbol::ArrR,   "]"},
            {Symbol::DfTp,   ":"},
		    {Symbol::Endd,   ";"},
            {Symbol::DfAt,  ":="},
		    {Symbol::Coma,   ","},
            {Symbol::CGet,   "'"},
		    {Symbol::RGet,   "."},
            {Symbol::Mayy,   "?"},
		    {Symbol::Stri,  "\""},
            {Symbol::CapS,   "`"},
		    {Symbol::Poin,   "$"},
            {Symbol::Bslh,  "\\"},
		    {Symbol::Rang,  ".."},
            {Symbol::Many, "..."},
		    {Symbol::Attt,   "@"},
    };

	inline ::std::map<::std::string const, Keyword const> const
	    str_keyword_map{
		    {    "if",     Keyword::If},
            {  "else",     Keyword::El},
		    {  "elif",   Keyword::Elif},
            { "while",  Keyword::While},
		    {   "for",    Keyword::For},
            {    "st",   Keyword::Step},
		    { "break",  Keyword::Break},
            {  "case",   Keyword::Case},
		    {    "vr",     Keyword::Vr},
            {    "fn",     Keyword::Fn},
		    {    "mc",     Keyword::Mc},
            {"inline", Keyword::Inline},
		    {  "with",   Keyword::With},
            {    "as",     Keyword::As},
    };

	inline ::std::map<Keyword const, ::std::string const> const
	    keyword_str_map{
		    {    Keyword::If,     "if"},
            {    Keyword::El,   "else"},
		    {  Keyword::Elif,   "elif"},
            { Keyword::While,  "while"},
		    {   Keyword::For,    "for"},
            {  Keyword::Step,     "st"},
		    { Keyword::Break,  "break"},
            {  Keyword::Case,   "case"},
		    {    Keyword::Vr,     "vr"},
            {    Keyword::Fn,     "fn"},
		    {    Keyword::Mc,     "mc"},
            {Keyword::Inline, "inline"},
		    {  Keyword::With,   "with"},
            {    Keyword::As,     "as"},
    };

}

namespace detail {

	inline ::std::string to_string(
	    WordTableItem::Whitespace const &value) {
		using Whitespace = WordTableItem::Whitespace;

		return ::std::format("Whitespace[{}]", [&] {
			switch (value) {
			case Whitespace::Space  : return "space";
			case Whitespace::Newline: return "newline";
			default                 : return "unknown";
			}
		}());
	}

	inline ::std::string to_string(WordTableItem::Symbol const &value) {
		return ::std::format("Symbol[{}]", [&] {
			auto pair = internal::symbol_str_map.find(value);
			auto is_known_symbol
			    = pair != internal::symbol_str_map.end();
			if (is_known_symbol) return pair->second;
			else return ::std::string{ "unknown" };
		}());
	}

	inline ::std::string to_string(
	    WordTableItem::Keyword const &value) {
		return ::std::format("Keyword[{}]", [&] {
			auto pair = internal::keyword_str_map.find(value);
			auto is_known_keyword
			    = pair != internal::keyword_str_map.end();
			if (is_known_keyword) return pair->second;
			else return ::std::string{ "unknown" };
		}());
	}

	inline ::std::string to_string(WordTableItem::Name const &value) {
		return ::std::format("Name[{}]", value.value);
	}

	inline ::std::string to_string(WordTableItem::String const &value) {
		return ::std::format("String[{}]", value.value);
	}

	inline ::std::string to_string(WordTableItem::Number const &value) {
		::std::string s;
		::std::visit(
		    [&](auto &&item) { s = ::std::format("Number[{}]", item); },
		    value);
		return s;
	}

	inline ::std::string to_string(WordTableItem::Unknow const &value) {
		return ::std::format("Unknow[{}]", value.text);
	}

	inline ::std::string to_string(
	    WordTableItem::ItemType const &item_value) {
		::std::string s;
		visit__start();
		if_T_is(nullptr_t) s = "null";
		elif_T_is(WordTableItem::Whitespace) s
		    = detail::to_string(item);
		elif_T_is(WordTableItem::Symbol) s  = detail::to_string(item);
		elif_T_is(WordTableItem::Keyword) s = detail::to_string(item);
		elif_T_is(WordTableItem::Name) s    = detail::to_string(item);
		elif_T_is(WordTableItem::String) s  = detail::to_string(item);
		elif_T_is(WordTableItem::Number) s  = detail::to_string(item);
		elif_T_is(WordTableItem::Unknow) s  = detail::to_string(item);
		else s = ::std::format("NoMaching[{}]", typeid(T).name());
		visit__end(item_value);
		return s;
	}

	inline ::std::string to_string(WordTableItem const &item) {
		::std::string s = ::std::format(
		    "{}: {}", ::std::string(item.position_range),
		    to_string(item.value));
		return s;
	}

}  //namespace detail

inline WordTableItem::operator ::std::string() const {
	return detail::to_string(*this);
}

}  //namespace xn::table

namespace std {

using ::xn::table::detail::to_string;

}
