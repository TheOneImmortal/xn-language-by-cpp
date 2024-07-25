#pragma once
/**
 * ****************tree_table.hh****************
 * @brief  语法分析表
 * @author https://github.com/TheOneImmortal
 * @date   2024-07-21
 * ********************************
 */

/*----------------include----------------*/
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "../useful_macro.hh"
#include "word_table.hh"

/*----------------body----------------*/
namespace xn::table {

inline i32 indent = 0;

// 如果name里是空是错误情况，如果没名字，我也会用”_“表示，用户命名为“_”即代表不想要名字，与空等价
struct TreeTableItem {
  private:
	using WordItem = table::WordTableItem;

  public:
	struct Name;
	struct BaseTemplatedName;
	using BaseName = ::std::variant<string, BaseTemplatedName>;

	struct BaseTemplatedName {
		string              name;
		::std::vector<Name> templates;
	};

	struct Name {
		reader::PosRange        pos;
		::std::vector<BaseName> spaces;
		BaseName                name;

		operator string() const {
			string s;
			visit__start();
			if_T_is(string) s              += item;
			elif_T_is(BaseTemplatedName) s += item.name + "<...>";
			visit__end(name);
			return s;
		}
	};

	struct Command {
		WordItem::Name content;
	};

	struct Operation;
	struct FnCall;
	struct CapString;
	struct Exp;

	struct FnCall {
		Name             fn_name;
		std::vector<Exp> args;
	};

	struct Operation {
		WordItem::Symbol       op;
		::std::unique_ptr<Exp> lhs, rhs;
	};

	struct CapString
	    : public ::std::vector<::std::variant<WordItem::String, Exp>> {
		operator string() const {
			return ::std::format("cap[{}]", [&] {
				string s;
				for (auto const &item : *this) {
					visit__start();
					if_T_is(WordItem::String) s += item.value;
					elif_T_is(Exp) s += "{" + to_string(item) + "}";
					visit__end(item);
				}
				return s;
			}());
		}
	};

	struct Exp
	    : public ::std::variant<Name, WordItem::String,
	                            WordItem::Number, FnCall, Operation,
	                            CapString> {};

	struct VarDef {
		::std::vector<Name>  name;
		Name                 type;
		::std::optional<Exp> value;

		operator string() const {
			return ::std::format("vr[{}]",
			                     [&] {
				                     string s;
				                     bool   is_first = true;
				                     for (auto const &var_name : name) {
					                     if (is_first) is_first = false;
					                     else s += ", ";
					                     s += string(var_name);
				                     }
				                     return s;
			                     }())
			     + (value.has_value() ? ::std::format(
			            " = {}", to_string(value.value()))
			                          : "");
		}
	};

	struct IfStream;
	struct WhileStream;
	struct ForStream;
	struct FnDef;
	struct Break;
	using Process = ::std::variant<FnDef, VarDef, Exp, IfStream,
	                               WhileStream, ForStream, Break>;
	using ProcessStream
	    = ::std::variant<Exp, IfStream, WhileStream, ForStream>;

	struct Break {};

	struct FnDef {
		Name                   name;
		::std::vector<VarDef>  params;
		::std::vector<Process> body;

		operator string() const {
			string s = ::std::format("{:{}}fn: {}\n", " ", indent,
			                         string(name));

			indent += 4;
			for (auto &p : body) s += to_string(p) + "\n";
			indent -= 4;

			return s;
		}
	};

	struct IfStream {
		::std::optional<Exp>             cond;
		::std::vector<Process>           body;
		::std::unique_ptr<ProcessStream> else_body;

		operator string() const {
			string s = ::std::format(
			    "{0:{1}}if({2}){{\n{3}{0:{1}}}}", " ", indent,
			    cond.has_value() ? to_string(cond.value()) : "void",
			    [&] {
				    string s = "";

				    indent += 4;
				    for (auto const &p : body) s += to_string(p) + "\n";
				    indent -= 4;

				    return s;
			    }());
			if (else_body) {
				s += "else ";
				visit__start();
				if_T_is(Exp) s        += to_string(item);
				elif_T_is(IfStream) s += string(item);
				else if constexpr (requires { string(item); }) {
					s += string(item);
				}
				visit__end(*else_body);
			}
			return s;
		}
	};

	struct WhileStream {
		::std::optional<Exp>             cond;
		::std::vector<Process>           body;
		::std::unique_ptr<ProcessStream> else_body;

		operator string() const {
			string s = ::std::format(
			    "{0:{1}}while({2}){{\n{3}{0:{1}}}}", " ", indent,
			    cond.has_value() ? to_string(cond.value()) : "void",
			    [&] {
				    string s = "";

				    indent += 4;
				    for (auto const &p : body) s += to_string(p) + "\n";
				    indent -= 4;

				    return s;
			    }());
			if (else_body) {
				s += "else ";
				visit__start();
				if_T_is(Exp) s        += to_string(item);
				elif_T_is(IfStream) s += string(item);
				else if constexpr (requires { string(item); }) {
					s += string(item);
				}
				visit__end(*else_body);
			}
			return s;
		}
	};

	struct ForStream {
		::std::optional<Exp>             range;
		::std::optional<Exp>             step;
		::std::vector<Name>              names;
		::std::optional<Name>            tag;
		::std::vector<Process>           body;
		::std::unique_ptr<ProcessStream> else_body;

		operator ::std::string() const {
			string s = ::std::format(
			    "{0:{1}}for({2}){{\n{3}{0:{1}}}}", " ", indent,
			    range.has_value() ? to_string(range.value()) : "void",
			    [&] {
				    string s = "";

				    indent += 4;
				    for (auto const &p : body) s += to_string(p) + "\n";
				    indent -= 4;

				    return s;
			    }());
			if (else_body) {
				s += "else ";
				visit__start();
				if_T_is(Exp) s        += to_string(item);
				elif_T_is(IfStream) s += string(item);
				else if constexpr (requires { string(item); }) {
					s += string(item);
				}
				visit__end(*else_body);
			}
			return s;
		}
	};

	::std::variant<Command, VarDef, FnDef> value;

	operator string() const {
		string s = "";
		visit__start();
		if_T_is(TreeTableItem::Command) s
		    += "cmdd: " + item.content.value;
		elif_T_is(TreeTableItem::VarDef) s += string(item);
		elif_T_is(TreeTableItem::FnDef) s  += string(item);
		else s += "unknown"; visit__end(value);
		return s;
	}

	static inline string to_string(Process const &process) {
		string s = "";
		visit__start();
		if_T_is(TreeTableItem::Exp) s += ::std::format(
		    "{:{}}exp: {}", " ", indent, to_string(item));
		elif_T_is(TreeTableItem::VarDef) s
		    += ::std::format("{:{}}{}", " ", indent, string(item));
		elif_T_is(TreeTableItem::FnDef) {
			s += ::std::format("{:{}}fn[\n{}\n{:{}}]", " ", indent,
			                   string(item), " ", indent);
		}
		elif_T_is(TreeTableItem::IfStream) s += string(item);
		else if constexpr (requires { string(item); }) {
			s += string(item);
		}
		else s += ::std::format("{:{}}unknown[{}]", " ", indent,
		                        typeid(item).name());
		visit__end(process);
		return s;
	}

	static inline string to_string(Exp const &exp) {
		string s = "";
		visit__start();
		if_T_is(Name) s += ::std::format("name[{}]", string(item));
		elif_T_is(WordItem::String) s
		    += ::std::format("string[{}]", item.value);
		elif_T_is(WordItem::Number) s
		    += ::std::format("number[{}]", string(item));
		elif_T_is(Operation) s
		    += ::std::format("operation{}({},{})", i32(item.op),
		                     item.lhs ? to_string(*item.lhs) : "void",
		                     item.rhs ? to_string(*item.rhs) : "void");
		elif_T_is(FnCall) s += ::std::format(
		    "fncall[{}]({})", string(item.fn_name), [&] {
			    string s        = "";
			    bool   is_first = true;
			    for (auto const &arg : item.args) {
				    if (is_first) is_first = false;
				    else s += ", ";
				    s += to_string(arg);
			    }
			    return s;
		    }());
		elif_T_is(CapString) s += string(item);
		else s += ::std::format("unknown[{}]", typeid(item).name());
		visit__end(exp);
		return s;
	}
};

struct TreeTable {
	::std::vector<TreeTableItem> items;
};

}  //namespace xn::table
