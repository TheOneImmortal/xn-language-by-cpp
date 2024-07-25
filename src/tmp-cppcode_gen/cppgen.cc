#include "cppgen.hh"

#include <utility>

#include "../log/log.hh"

namespace xn::cppgen {

using Item = table::TreeTableItem;
using Word = table::WordTableItem;

::std::vector<string> split(string str, char separator) {
	::std::vector<string> ret;

	int startIndex = 0, endIndex = 0;
	for (int i = 0; i <= str.size(); i++) {
		// If we reached the end of the word or the end of the input.
		if (str[i] == separator || i == str.size()) {
			endIndex    = i;
			string temp = str.substr(startIndex, endIndex - startIndex);
			if (!temp.empty()) ret.push_back(::std::move(temp));
			startIndex = endIndex + 1;
		}
	}

	return ret;
}

#define FN_to_cpp(nodetype) string to_cpp(Item::nodetype const &node)

auto to_cpp(auto const &v) -> decltype(v) {
	log::error_log("link error type [{}]", typeid(v).name());
}

FN_to_cpp(BaseTemplatedName);
FN_to_cpp(BaseName);
FN_to_cpp(Name);
FN_to_cpp(Exp);
FN_to_cpp(FnCall);
FN_to_cpp(Operation);
FN_to_cpp(CapString);
FN_to_cpp(VarDef);
FN_to_cpp(FnDef);
FN_to_cpp(Break);
FN_to_cpp(IfStream);
FN_to_cpp(WhileStream);
FN_to_cpp(ForStream);

string to_cpp(Word::String const &node) {
	return "\"" + node.value + "\"";
}

string to_cpp(Word::Number const &node) {
	::std::string s;
	visit__start();
	s = ::std::to_string(item);
	visit__end(node);
	return s;
}

FN_to_cpp(BaseTemplatedName) {
	::std::string ret = "";
	ret              += "__xn" + node.name;
	if (!node.templates.empty()) {
		ret += "__tp";
		for (auto const &tem : node.templates) ret += to_cpp(tem);
		ret += "__pt";
	}
	return ret;
}

FN_to_cpp(BaseName) {
	string ret = "";
	visit__start();
	if_T_is(string) { ret += "__xn" + item; }
	elif_T_is(Item::BaseTemplatedName) { ret += to_cpp(item); }
	visit__end(node);
	return ret;
}

FN_to_cpp(Name) {
	string ret = "";
	for (auto const &space : node.spaces) ret += to_cpp(space);
	ret += to_cpp(node.name);
	return ret;
}

FN_to_cpp(Exp) {
	string ret = "";
	visit__start();
	if constexpr (requires { to_cpp(item); }) {
		ret += to_cpp(item);
	} else {
		log::error_log("exp: unknown node type [{}]",
		               typeid(item).name());
	}
	visit__end(node);
	return ret;
}

FN_to_cpp(FnCall) {
	return ::std::format("{}({})", to_cpp(node.fn_name), [&] -> string {
		if (node.args.empty()) return "";
		string s        = "";
		bool   is_first = true;
		for (auto const &arg : node.args) {
			if (!is_first) s += ",";
			is_first = false;
			s       += to_cpp(arg);
		}
		return s;
	}());
}

#define OPERATION_CASE__ONE(symbol, op)                        \
	case Word::Symbol::symbol: {                               \
		if (!node.rhs) {                                       \
			log::error_log("error: {} has no rhs", op);        \
			return "NULL";                                     \
		}                                                      \
		return ::std::format("({}{})", op, to_cpp(*node.rhs)); \
	} break
#define OPERATION_CASE__TWO(symbol, op)                         \
	case Word::Symbol::symbol: {                                \
		if (!node.lhs) {                                        \
			log::error_log("error: {} has no lhs", op);         \
			return "NULL";                                      \
		}                                                       \
		if (!node.rhs) {                                        \
			log::error_log("error: {} has no rhs", op);         \
			return "NULL";                                      \
		}                                                       \
		return ::std::format("({}{}{})", to_cpp(*node.lhs), op, \
		                     to_cpp(*node.rhs));                \
	} break
#define OPERATION_CASE__ONE_OR_TWO(symbol, op)                      \
	case Word::Symbol::symbol: {                                    \
		if (!node.rhs) {                                            \
			log::error_log("error: {} has no rhs", op);             \
			return "NULL";                                          \
		}                                                           \
		if (node.lhs)                                               \
			return ::std::format("({}{}{})", to_cpp(*node.lhs), op, \
			                     to_cpp(*node.rhs));                \
		else return ::std::format("({}{})", op, to_cpp(*node.rhs)); \
	} break

FN_to_cpp(Operation) {
	switch (node.op) {
		OPERATION_CASE__ONE(Oppo, '~');
		OPERATION_CASE__ONE(Nott, '!');
		OPERATION_CASE__ONE(PluS, "++");
		OPERATION_CASE__ONE(MinS, "--");
		OPERATION_CASE__ONE_OR_TWO(Plus, '+');
		OPERATION_CASE__ONE_OR_TWO(Minu, '-');
		OPERATION_CASE__TWO(Muti, '*');
		OPERATION_CASE__TWO(Divd, '/');
		OPERATION_CASE__TWO(Modu, '%');
		OPERATION_CASE__TWO(Orrr, '|');
		OPERATION_CASE__TWO(Andd, '&');
		OPERATION_CASE__TWO(Xorr, '^');
		OPERATION_CASE__TWO(LMov, "<<");
		OPERATION_CASE__TWO(RMov, ">>");
		OPERATION_CASE__TWO(PluE, "+=");
		OPERATION_CASE__TWO(MinE, "-=");
		OPERATION_CASE__TWO(MutE, "*=");
		OPERATION_CASE__TWO(DivE, "/=");
		OPERATION_CASE__TWO(ModE, "%=");
		OPERATION_CASE__TWO(OrrE, "|=");
		OPERATION_CASE__TWO(AndE, "&=");
		OPERATION_CASE__TWO(XorE, "^=");
		OPERATION_CASE__TWO(OppE, "~=");
		OPERATION_CASE__TWO(NotE, "!=");
		OPERATION_CASE__TWO(LMoE, "<<=");
		OPERATION_CASE__TWO(RMoE, ">>=");
		OPERATION_CASE__TWO(Equl, "=");
		OPERATION_CASE__TWO(Same, "==");
		OPERATION_CASE__TWO(LeEq, "<=");
		OPERATION_CASE__TWO(GrEq, ">=");
	case Word::Symbol::Rang: {
		if (!node.lhs) {
			log::error_log("error: .. has no lhs");
			return "NULL";
		}
		if (!node.rhs) {
			log::error_log("error: .. has no rhs");
			return "NULL";
		}
		return ::std::format("__xnrange({}, {})", to_cpp(*node.lhs),
		                     to_cpp(*node.rhs));
	} break;
	default: log::error_log("unknown operation: {}", (i32) node.op);
	}
	return "NULL";
}

FN_to_cpp(CapString) {
	return ::std::format("__xnto_string({})", [&] {
		string s;
		bool   is_first = true;
		for (auto const &c : node) {
			if (!is_first) s += ",";
			else is_first = false;
			visit__start();
			if_T_is(Word::String) s += "\"" + item.value + "\"";
			else s                  += to_cpp(item); visit__end(c);
		}
		return s;
	}());
}

FN_to_cpp(VarDef) {
	return ::std::format(
	    "{} {}{}",
	    [&] -> string {
		    string s = to_cpp(node.type);
		    if (s == "__xn_") return "auto";
		    else return s;
	    }(),
	    [&] {
		    string s        = "";
		    bool   is_first = true;
		    for (auto const &name : node.name) {
			    if (!is_first) s += ",";
			    is_first = false;
			    s       += to_cpp(name);
		    }
		    return s;
	    }(),
	    node.value ? "=" + to_cpp(node.value.value()) : "");
}

FN_to_cpp(FnDef) {
	string ret = "";

	if (string(node.name) == "main") ret = "int main(){\n";
	else ret = "void " + to_cpp(node.name) + "(){\n";

	for (auto const &stmt : node.body) {
		visit__start();
		if constexpr (::std::is_same_v<string,
		                               decltype(to_cpp(item))>) {
			ret += to_cpp(item) + ";";
		} else {
			log::error_log("unknown node type [{}]",
			               typeid(item).name());
		}

		visit__end(stmt);
	}

	ret += "}\n";
	return ret;
}

FN_to_cpp(Break) { return "break;"; }

FN_to_cpp(IfStream) {
	return ::std::format(
	    "if({}){{{}}}{}",
	    node.cond ? to_cpp(node.cond.value()) : "true",
	    [&] {
		    string s;
		    for (auto const &p : node.body) {
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(p);
		    }
		    return s;
	    }(),
	    [&] {
		    string s;
		    if (node.else_body) {
			    s += "else {";
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(*node.else_body);
			    s += "}";
		    }
		    return s;
	    }());
}

FN_to_cpp(WhileStream) {
	return ::std::format(
	    "{{bool is_success = false; while(true){{bool is_true = "
	    "{};if(is_true){{{}}}else{{ is_success = true;break;}}}}{}}}",
	    node.cond ? to_cpp(node.cond.value()) : "true",
	    [&] {
		    string s;
		    for (auto const &p : node.body) {
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(p);
		    }
		    return s;
	    }(),
	    [&] {
		    string s;
		    if (node.else_body) {
			    s += "if(is_success){";
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(*node.else_body);
			    s += "}";
		    }
		    return s;
	    }());
}

FN_to_cpp(ForStream) {
	// FIXME:
	// 明显有问题，阿巴阿巴，需要让语言先有自定义类型才能解决呢。回头再说吧
	return ::std::format(
	    "{{bool is_success=false;__xni32 j=0;for(auto i:"
	    "{}){{do{{j++;is_success=false;{}{}{}is_success=true;}}while(0)"
	    ";}}{}}}",
	    node.range ? to_cpp(node.range.value()) : "NULL",
	    ((node.names.size() >= 1)
	         ? ::std::format("auto &{} = i;", to_cpp(node.names[0]))
	         : ""),
	    ((node.names.size() >= 2)
	         ? ::std::format("auto &{} = j;", to_cpp(node.names[1]))
	         : ""),
	    [&] {
		    string s;
		    for (auto const &p : node.body) {
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(p);
		    }
		    return s;
	    }(),
	    [&] {
		    string s;
		    if (node.else_body) {
			    s += "if(is_success){";
			    visit__start();
			    s += to_cpp(item) + ";";
			    visit__end(*node.else_body);
			    s += "}";
		    }
		    return s;
	    }());
}

// FN_to_cpp(VarDef) {
// 	log::error_log("VarDef: not implemented");
// 	return "";
// }

void CppGen::gen(Item::Command const &node) {
	auto const &value = node.content.value;
	auto        strs  = split(value, ' ');
	if (strs.size() >= 1) {
		if (strs[0] == "import") {
			if (strs.size() >= 2)
				if (strs[1] == "io") {
					output << R"(#include <iostream>
template<typename...Ts>constexpr auto __xnio__xnout(Ts const&...v){
((::std::cout << v), ...);::std::cout << ::std::endl;};
__xni32 __xnio__xnin__tp__xni32__pt(){__xni32 __xnv;::std::cin>>__xnv;return __xnv;})"
					       << ::std::endl;
				} else
					log::error_log(
					    "import command unknown argument [{}]",
					    strs[1]);
			else log::error_log("import command missing argument");
		} else {
			log::error_log("unknown command");
		}
	}
}

void CppGen::gen(Item::FnDef const &node) {
	if (string(node.name) == "main")
		output << "int main(){" << ::std::endl;
	else output << "void " << to_cpp(node.name) << "(){" << ::std::endl;

	for (auto const &stmt : node.body) {
		visit__start();
		if constexpr (::std::is_same_v<string,
		                               decltype(to_cpp(item))>) {
			output << to_cpp(item) << ";" << ::std::endl;
		} else {
			log::error_log("unknown node type [{}]",
			               typeid(item).name());
		}

		visit__end(stmt);
	}

	output << "}" << ::std::endl;
}

void CppGen::gen() {
	output << R"(#include <iterator>
#include <sstream>
using __xni32=int;
template<typename T>struct __xnRange__xnIter{
using value_type=T;
using pointer=T*;
using reference=T&;
using difference_type=ptrdiff_t;
using iterator_category=std::input_iterator_tag;
explicit __xnRange__xnIter(T begin):v(begin){}
reference operator*(){return v;}
__xnRange__xnIter&operator++(){++v;return*this;}
bool operator==(__xnRange__xnIter const&other)const{return v==other.v;}
bool operator!=(__xnRange__xnIter const&other)const{return!(v==other.v);}
T v;};
template<typename T>struct __xnRange{T l,r;
__xnRange__xnIter<T>begin(){return __xnRange__xnIter<T>(l);}
__xnRange__xnIter<T>end(){return __xnRange__xnIter<T>(r);}};
template<typename T>__xnRange<T>__xnrange(T const&l, T const&r){
return l<=r?__xnRange<T>{l,r}:__xnRange<T>{r,l};}
template<typename...Ts>auto __xnto_string(Ts const&...v){
::std::ostringstream __xnoss;((__xnoss<<v),...);return __xnoss.str();};)"
	       << ::std::endl;

	for (auto const &node : tree_table.items) {
		visit__start();
		if constexpr (requires { gen(item); }) {
			gen(item);
		} else {
			log::error_log("cppgen: unknown node type [{}]",
			               typeid(item).name());
		}
		visit__end(node.value);
	}
}

void cpp_gen(string output_file, table::TreeTable const &tree_table) {
	CppGen cppgen{ output_file, tree_table };
	cppgen.gen();
}

}  //namespace xn::cppgen