#include "tree.hh"

#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

#include "../log/log.hh"
#include "../tables/word_table.hh"
#include "../useful_macro.hh"

namespace xn::tree {

using WordItem = table::WordTableItem;
using Words    = ::std::vector<WordItem>;
using TreeItem = table::TreeTableItem;
using Table    = table::TreeTable;

#define visit_word_i__start() \
	if (i < words.size()) {   \
	visit__start()

#define expect_word_i_faild(state, content)                      \
	log::error_log(                                              \
	    "When [{}], Expect [{}], but get: [{}]", state, content, \
	    (i < words.size()) ? ::std::to_string(words[i]) : "EOF")

#define visit_word_i__end()     \
	visit__end(words[i].value); \
	}                           \
	else

void Tree::try_skip_whitespace(bool const is_skip_line) {
	bool is_continue = true;
	while (is_continue) {
		visit_word_i__start();
		if_T_is(WordItem::Whitespace) {
			if (is_skip_line || item != WordItem::Whitespace::Newline)
				i++;
			else is_continue = false;
		}
		else { is_continue = false; }
		visit_word_i__end() { break; }
	}
}

/**
 * @brief 尝试跳过期望内容，也会跳过空白、换行
 *
 * 如果 当前内容是期望内容，则跳过，否则返回false
 */
template<typename ExpectT>
bool Tree::try_skip_word(bool const is_skip_line, auto &&f)
    requires(
        ::std::is_invocable_r_v<bool, decltype(f), ExpectT const &>)
{
	bool ret = false;
	try_skip_whitespace(is_skip_line);
	visit_word_i__start();
	if_T_is(ExpectT) {
		if (f(item)) {
			ret = true;
			i++;
		}
	}
	visit_word_i__end();
	return ret;
}

void Tree::just_skip_line() {
	bool is_continue = true;
	while (is_continue) {
		visit_word_i__start();
		if_T_is(WordItem::Whitespace) {
			if (item != WordItem::Whitespace::Newline) i++;
			else is_continue = false;
		}
		else { i++; }
		visit_word_i__end() { break; }
	}
}

template<typename ExpectT>
bool Tree::just_skip_word(bool const is_skip_line, auto &&f)
    requires(
        ::std::is_invocable_r_v<bool, decltype(f), ExpectT const &>)
{
	bool ret = false;

	bool is_continue = true;
	while (is_continue) {
		visit_word_i__start();
		if_T_is(WordItem::Whitespace) {
			if (is_skip_line || item != WordItem::Whitespace::Newline)
				i++;
			else is_continue = false;
		}
		elif_T_is(ExpectT) {
			i++;
			if (f(item)) {
				ret         = true;
				is_continue = false;
			}
		}
		else { i++; }
		visit_word_i__end() { break; }
	}

	return ret;
}

bool Tree::try_skip_symbol(bool const             is_skip_line,
                           WordItem::Symbol const expect) {
	return try_skip_word<WordItem::Symbol>(
	    is_skip_line, [&](WordItem::Symbol const &item) -> bool {
		    return item == expect;
	    });
}

bool Tree::just_skip_symbol(bool const             is_skip_line,
                            WordItem::Symbol const expect) {
	return just_skip_word<WordItem::Symbol>(
	    is_skip_line, [&](WordItem::Symbol const &item) -> bool {
		    return item == expect;
	    });
}

::std::optional<TreeItem::Name> Tree::try_build_name() {
	::std::optional<reader::PosRange> pos;
	::std::vector<TreeItem::BaseName> names;

	bool is_continue = true;
	while (is_continue && i < words.size()) {
		visit_word_i__start();
		if_T_is(WordItem::Name) {
			if (!pos.has_value()) pos = words[i].position_range;
			else pos->end = words[i].position_range.end;
			i++;

			if (try_skip_symbol(false, WordItem::Symbol::Less)) {
				names.push_back(TreeItem::BaseTemplatedName{
				    .name = item.value, .templates = [this] {
					    ::std::vector<TreeItem::Name> names;

					    while (true) {
						    auto name = try_build_name();
						    if (!name.has_value()) break;
						    names.push_back(*name);

						    if (!try_skip_symbol(
						            true, WordItem::Symbol::Coma))
							    break;
					    }
					    return names;
				    }() });

				if (!try_skip_symbol(true, WordItem::Symbol::Gret))
					expect_word_i_faild("分析模板名", ">");
			} else {
				names.push_back(item.value);
			}

			is_continue
			    = try_skip_symbol(false, WordItem::Symbol::CGet);
		}
		elif_T_is(WordItem::Whitespace) { i++; }
		else {
			is_continue = false;
			if (names.empty()) expect_word_i_faild("分析名字", "name");
		}
		visit_word_i__end() { break; }
	}

	if (!pos.has_value()) return ::std::nullopt;

	return TreeItem::Name{ .pos    = *pos,
		                   .spaces = ::std::vector<TreeItem::BaseName>(
		                       names.begin(), names.end() - 1),
		                   .name = *(names.end() - 1) };
}

void Tree::build_命令(WordItem::Symbol const &now, Table &table) {
	visit_word_i__start();
	if_T_is(WordItem::Name) {
		log::debug_log("cmdd found...");
		table.items.push_back(
		    TreeItem{ TreeItem::Command{ .content = { item.value } } });
		i++;
	}
	else { expect_word_i_faild("构造命令", "cmdd"); }
	visit_word_i__end();
}

// TreeItem::Tuple try_build_typed_tuple(int &i, Table &table,
//                                       Words const &words) {}

// void build_fndef(int &i, Table &table, Words const &words) {
// 	auto fn_name = try_build_name();
// 	if (!fn_name.has_value())
// 		fn_name = TreeItem::Name{ .pos  = words[i].position_range.start,
// 			                      .name = "_" };

// 	try_build_typed_tuple(i, table, words);

// 	table.items.push_back(TreeItem{
// 	    TreeItem::FnDef{
// 	                    .fn_name = *fn_name,
// 	                    .params  = {},
// 	                    }
//     });
// }

::std::optional<TreeItem::CapString> Tree::try_build_捕获字符串() {
	TreeItem::CapString cap_string{};

	// 形如`String{表达式}String{表达式}String`
	bool is_continue = true;
	while (is_continue) {
		visit_word_i__start();
		if_T_is(WordItem::String) {
			i++;
			cap_string.push_back(item);
		}
		elif_T_is(WordItem::Symbol) {
			if (item == WordItem::Symbol::SwiL) {
				i++;
				auto exp = try_build_表达式();
				if (exp.has_value())
					cap_string.push_back(::std::move(exp.value()));
				is_continue
				    = try_skip_symbol(true, WordItem::Symbol::SwiR);
			} else if (item == WordItem::Symbol::CapS) {
				i++;
				is_continue = false;
			} else {
				is_continue = false;
				expect_word_i_faild("构造捕获字符串", "`|{}");
			}
		}
		else {
			is_continue = false;
			expect_word_i_faild("构造捕获字符串", "`|{}");
		}
		visit_word_i__end() { break; }
	}

	return cap_string;
}

::std::optional<TreeItem::Exp> Tree::try_build_表达式() {
	::std::optional<TreeItem::Exp> ret;

	try_skip_whitespace(true);

	visit_word_i__start();
	if_T_is(WordItem::Name) {
		auto name = try_build_name();
		if (!name) {
			expect_word_i_faild("构造表达式", "name");
			return;
		}
		if (try_skip_symbol(false,
		                    WordItem::Symbol::ExpL)) {  // (, 是函数调用
			TreeItem::FnCall fn_call{ .fn_name = name.value() };

			log::debug_log("fn[{}] call...", string(name.value()));
			// 读取参数
			if (try_skip_symbol(true, WordItem::Symbol::ExpR)) {
				ret = (TreeItem::Exp)::std::move(fn_call);
			} else {
				while (true) {
					auto param = try_build_表达式();
					if (!param.has_value()) {
						expect_word_i_faild("构造函数调用参数", "exp");
						break;
					}
					fn_call.args.push_back(::std::move(param.value()));

					if (!try_skip_symbol(false, WordItem::Symbol::Coma))
						break;
					try_skip_whitespace(true);
				}

				if (try_skip_symbol(true, WordItem::Symbol::ExpR))
					ret = (TreeItem::Exp)::std::move(fn_call);
			}
			log::debug_log("fn[{}] call finish", string(name.value()));
		} else {
			ret = (TreeItem::Exp)::std::move(name.value());
		}
	}
	elif_T_is(WordItem::String) {
		i++;
		ret = (TreeItem::Exp) WordItem::String(item.value);
	}
	elif_T_is(WordItem::Symbol) {
		switch (item) {
		case WordItem::Symbol::ExpL: {
			i++;
			auto exp = try_build_表达式();
			if (exp.has_value()) ret = ::std::move(exp.value());
			if (!try_skip_symbol(true, WordItem::Symbol::ExpR))
				expect_word_i_faild("构造表达式", "exp的)");
		} break;
		case WordItem::Symbol::CapS: {
			i++;
			auto cap_string = try_build_捕获字符串();

			ret = (TreeItem::Exp)::std::move(cap_string.value());
		} break;
#define BUILD_ONE_OP(symbol)                                   \
	case WordItem::Symbol::symbol: {                           \
		i++;                                                   \
		auto exp = try_build_表达式();                         \
		ret      = (TreeItem::Exp) TreeItem::Operation{        \
			     .op  = WordItem::Symbol::symbol,              \
			     .rhs = exp.has_value()                        \
			              ? ::std::make_unique<TreeItem::Exp>( \
                         ::std::move(exp.value()))        \
			              : nullptr                            \
		};                                                     \
	} break
			BUILD_ONE_OP(Plus);
			BUILD_ONE_OP(Minu);
			BUILD_ONE_OP(PluS);
			BUILD_ONE_OP(MinS);
			BUILD_ONE_OP(Oppo);
			BUILD_ONE_OP(Nott);
		case WordItem::Symbol::ExpR:
		// 也许是某个Exp的右括号需求，直接忽略不管
		case WordItem::Symbol::DfTp:
		// 也许是流控制起始符，忽略不管
		case WordItem::Symbol::Endd:
			// 也许是某个流的结束，忽略不管
			break;
		default: expect_word_i_faild("构造表达式", "exp");
		}
	}
	elif_T_is(WordItem::Number) {
		i++;
		ret = (TreeItem::Exp) item;
	}
	else { expect_word_i_faild("构造表达式", "exp"); }
	visit_word_i__end();

	visit_word_i__start();
	if_T_is(WordItem::Symbol) {
		if (item == WordItem::Symbol::Plus
		    || item == WordItem::Symbol::Minu
		    || item == WordItem::Symbol::Muti
		    || item == WordItem::Symbol::Divd
		    || item == WordItem::Symbol::Modu
		    || item == WordItem::Symbol::Orrr
		    || item == WordItem::Symbol::Andd
		    || item == WordItem::Symbol::Xorr
		    || item == WordItem::Symbol::LMov
		    || item == WordItem::Symbol::RMov
		    || item == WordItem::Symbol::PluE
		    || item == WordItem::Symbol::MinE
		    || item == WordItem::Symbol::MutE
		    || item == WordItem::Symbol::DivE
		    || item == WordItem::Symbol::ModE
		    || item == WordItem::Symbol::OrrE
		    || item == WordItem::Symbol::AndE
		    || item == WordItem::Symbol::XorE
		    || item == WordItem::Symbol::OppE
		    || item == WordItem::Symbol::NotE
		    || item == WordItem::Symbol::LMoE
		    || item == WordItem::Symbol::RMoE
		    || item == WordItem::Symbol::Equl
		    || item == WordItem::Symbol::Same
		    //|| item == WordItem::Symbol::Less
		    //|| item == WordItem::Symbol::Gret
		    || item == WordItem::Symbol::LeEq
		    || item == WordItem::Symbol::GrEq
		    || item == WordItem::Symbol::Powr
		    || item == WordItem::Symbol::RoOp
		    || item == WordItem::Symbol::Rang) {
			i++;

			auto rhs = try_build_表达式();

			ret = (TreeItem::Exp) TreeItem::Operation{
				.op  = item,
				.lhs = ret.has_value()
				         ? ::std::make_unique<TreeItem::Exp>(
				             ::std::move(ret.value()))
				         : nullptr,
				.rhs = rhs.has_value()
				         ? ::std::make_unique<TreeItem::Exp>(
				             ::std::move(rhs.value()))
				         : nullptr
			};
		}
	}
	visit_word_i__end();

	return ret;
}

::std::optional<TreeItem::VarDef> Tree::try_build_变量定义() {
	::std::optional<TreeItem::VarDef> ret;

	::std::vector<TreeItem::Name> var_names;

	auto var_name = try_build_name();
	if (var_name.has_value())
		var_names.push_back(::std::move(var_name.value()));

	while (try_skip_symbol(false, WordItem::Symbol::Coma)) {
		try_skip_whitespace(true);
		auto var_name = try_build_name();
		if (var_name.has_value())
			var_names.push_back(::std::move(var_name.value()));
	}

	visit_word_i__start();
	if_T_is(WordItem::Symbol) {
		if (item == WordItem::Symbol::DfTp) {
			// FIXME: 处理显式类型定义
			expect_word_i_faild(
			    "构造变量定义",
			    "应当处理显式类型定义，我还没写相关代码");
		}
		if (item == WordItem::Symbol::DfAt) {
			i++;
			auto exp = try_build_表达式();
			if (exp.has_value())
				ret = TreeItem::VarDef{ .name = ::std::move(var_names),
					                    .type
					                    = TreeItem::Name{ .name = "_" },
					                    .value
					                    = ::std::move(exp.value()) };
		} else expect_word_i_faild("构造变量定义", ":=");
	}
	if_T_is(WordItem::Whitespace) {
		// FIXME: undef vr
		expect_word_i_faild("构造变量定义",
		                    "应当处理未undefvr，我还没写相关代码");
	}
	visit_word_i__end() expect_word_i_faild("构造变量定义", ":=");

	return ret;
}

::std::unique_ptr<TreeItem::ProcessStream> Tree::try_build_else流() {
	::std::unique_ptr<TreeItem::ProcessStream> ret;

	try_skip_whitespace(false);
	visit_word_i__start();
	if_T_is(WordItem::Keyword) {
		switch (item) {
		case WordItem::Keyword::If: {
			i++;
			auto else_if = try_build_条件流控制();
			if (else_if.has_value()) {
				ret = ::std::make_unique<TreeItem::ProcessStream>(
				    ::std::move(else_if.value()));
			}
		} break;
		case WordItem::Keyword::While: {
			i++;
			auto while_ = try_build_while循环流控制();
			if (while_.has_value()) {
				ret = ::std::make_unique<TreeItem::ProcessStream>(
				    ::std::move(while_.value()));
			}
		} break;
		case WordItem::Keyword::For: {
			i++;
			auto for_ = try_build_for循环流控制();
			if (for_.has_value()) {
				ret = ::std::make_unique<TreeItem::ProcessStream>(
				    ::std::move(for_.value()));
			}
		} break;
		default: expect_word_i_faild("构造流控制else", "if|exp");
		}
	}
	elif_T_is(WordItem::Symbol) {
		if (item == WordItem::Symbol::DfTp) {
			auto else_if = try_build_条件流控制();
			if (else_if.has_value()) {
				ret = ::std::make_unique<TreeItem::ProcessStream>(
				    ::std::move(else_if.value()));
			}
		} else {
			expect_word_i_faild("构造流控制else", "if|exp");
		}
	}
	elif_T_is(WordItem::Whitespace) {
		// 空白，说明没有else
	}
	else {
		auto exp = try_build_表达式();
		if (exp.has_value()) {
			ret = ::std::make_unique<TreeItem::ProcessStream>(
			    ::std::move(exp.value()));
		} else expect_word_i_faild("构造流控制else", "if|exp");
	}
	visit_word_i__end();

	return ret;
}

::std::optional<TreeItem::IfStream> Tree::try_build_条件流控制() {
	TreeItem::IfStream ret{ .cond = try_build_表达式() };

	if (try_skip_symbol(false, WordItem::Symbol::DfTp)) {
		bool is_continue = true;
		while (is_continue) {
			auto process = try_build_语句();
			if (process.has_value())
				ret.body.push_back(::std::move(process.value()));
			else is_continue = false;
		}
		if (try_skip_symbol(true, WordItem::Symbol::Endd))
			ret.else_body = try_build_else流();
	} else expect_word_i_faild("构造条件流控制", ":");

	return ::std::make_optional(::std::move(ret));
}

::std::optional<TreeItem::WhileStream>
Tree::try_build_while循环流控制() {
	TreeItem::WhileStream ret{ .cond = try_build_表达式() };

	if (try_skip_symbol(false, WordItem::Symbol::DfTp)) {
		bool is_continue = true;
		while (is_continue) {
			auto process = try_build_语句();
			if (process.has_value())
				ret.body.push_back(::std::move(process.value()));
			else is_continue = false;
		}
		if (try_skip_symbol(true, WordItem::Symbol::Endd))
			ret.else_body = try_build_else流();
	} else expect_word_i_faild("构造条件流控制", ":");

	return ::std::make_optional(::std::move(ret));
}

::std::optional<TreeItem::ForStream> Tree::try_build_for循环流控制() {
	TreeItem::ForStream ret{ .range = try_build_表达式() };

	if (!ret.range.has_value()) {
		expect_word_i_faild("构造for循环流控制", "exp");
		just_skip_symbol(true, WordItem::Symbol::Endd);
		return ::std::nullopt;
	}

	bool is_stepd = false;
	bool is_named = false;
	bool is_tagd  = false;

	bool is_continue = true;
	while (is_continue) {
		visit_word_i__start();
		if_T_is(WordItem::Keyword) {
			switch (item) {
			case WordItem::Keyword::Step: {
				if (is_stepd) {
					expect_word_i_faild("构造for循环流控制",
					                    "已step！");
					i++;
					try_build_表达式();
					break;
				}
				is_stepd = true;
				i++;
				auto exp = try_build_表达式();
				if (exp.has_value())
					ret.step = ::std::move(exp.value());
				else expect_word_i_faild("构造for循环流控制", "exp");
			} break;
			case WordItem::Keyword::As: {
				if (is_named) {
					expect_word_i_faild("构造for循环流控制", "已as！");
					i++;
					while (true) {
						auto var_name = try_build_name();
						if (!var_name.has_value()) break;
						if (!try_skip_symbol(false,
						                     WordItem::Symbol::Coma))
							break;
						try_skip_whitespace(true);
					}
					break;
				}
				is_named = true;
				i++;
				while (true) {
					auto var_name = try_build_name();
					if (!var_name.has_value()) {
						expect_word_i_faild("构造for的as名", "name");
						break;
					}
					ret.names.push_back(::std::move(var_name.value()));
					if (!try_skip_symbol(false, WordItem::Symbol::Coma))
						break;
					try_skip_whitespace(true);
				}
			} break;
			default: {
				expect_word_i_faild("构造for循环流控制", "st|as");
				is_continue = false;
			}
			}
		}
		elif_T_is(WordItem::Symbol) {
			if (item == WordItem::Symbol::DfTp) is_continue = false;
			else {
				expect_word_i_faild("构造for循环流控制", "st|as");
				is_continue = false;
			}
		}
		elif_T_is(WordItem::Whitespace) { i++; }
		else {
			expect_word_i_faild("构造for循环流控制", "st|as");
			is_continue = false;
		}
		visit_word_i__end() { break; }
	}

	if (try_skip_symbol(false, WordItem::Symbol::DfTp)) {
		bool is_continue = true;
		while (is_continue) {
			auto process = try_build_语句();
			if (process.has_value())
				ret.body.push_back(::std::move(process.value()));
			else is_continue = false;
		}
		if (try_skip_symbol(true, WordItem::Symbol::Endd))
			ret.else_body = try_build_else流();
	} else expect_word_i_faild("构造条件流控制", ":");

	return ret;
}

::std::optional<TreeItem::Process> Tree::try_build_语句() {
	::std::optional<TreeItem::Process> ret;

	try_skip_whitespace(true);

	visit_word_i__start();
	if_T_is(WordItem::Name) {
		auto exp = try_build_表达式();
		if (exp.has_value()) ret = ::std::move(exp.value());
	}
	elif_T_is(WordItem::Keyword) {
		switch (item) {
		case WordItem::Keyword::Vr: {
			i++;
			auto var_def = try_build_变量定义();
			if (var_def.has_value()) ret = ::std::move(var_def.value());
		} break;
		case WordItem::Keyword::If: {
			i++;
			auto if_stream = try_build_条件流控制();
			if (if_stream.has_value())
				ret = ::std::move(if_stream.value());
		} break;
		case WordItem::Keyword::For: {
			i++;
			auto for_stream = try_build_for循环流控制();
			if (for_stream.has_value())
				ret = ::std::move(for_stream.value());
		} break;
		case WordItem::Keyword::While: {
			i++;
			auto while_stream = try_build_while循环流控制();
			if (while_stream.has_value())
				ret = ::std::move(while_stream.value());
		} break;
		case WordItem::Keyword::Break: {
			i++;
			ret = TreeItem::Break{};
		} break;
		default: expect_word_i_faild("构造语句", "流控制");
		}
	}
	elif_T_is(WordItem::Symbol) {
		if (item == WordItem::Symbol::Plus
		    || item == WordItem::Symbol::Minu
		    || item == WordItem::Symbol::PluS
		    || item == WordItem::Symbol::MinS) {
			auto exp = try_build_表达式();
			if (exp.has_value()) ret = ::std::move(exp.value());
		} else if (item != WordItem::Symbol::Endd)
			expect_word_i_faild("构造语句", ";");
	}
	else { expect_word_i_faild("构造语句", "; or exp or def"); }
	visit_word_i__end();

	return ret;
}

void Tree::build_函数定义(Table &table) {
	auto fn_name = try_build_name();
	if (!fn_name.has_value())
		fn_name = TreeItem::Name{ .pos  = words[i].position_range.start,
			                      .name = "_" };

	log::debug_log("fn[{}] def...", string(fn_name.value()));
	if (!try_skip_symbol(true, WordItem::Symbol::ExpL)) {
		expect_word_i_faild("分析函数签名", "(");
		return;
	}

	try_skip_whitespace(true);

	// FIXME: read params

	if (!try_skip_symbol(true, WordItem::Symbol::ExpR)) {
		expect_word_i_faild("分析函数签名", ")");
		return;
	}
	if (!try_skip_symbol(false, WordItem::Symbol::DfTp)) {
		expect_word_i_faild("分析函数签名", ":");
		return;
	}

	TreeItem::FnDef fn_def{ .name = fn_name.value() };

	bool is_continue = true;
	while (is_continue) {
		auto process = try_build_语句();
		if (process.has_value())
			fn_def.body.push_back(::std::move(process.value()));
		else is_continue = false;
	}

	if (!try_skip_symbol(true, WordItem::Symbol::Endd)) {
		log::debug_log("fn[{}] def error", string(fn_name.value()));
		expect_word_i_faild("分析函数语句", "各种语句");
		// TODO:
		// 这里应该跳过函数定义，直到函数结束，但是直接匹配';'可能会把流控制当作函数结束，所以这里需要更复杂的跳过逻辑。
		//不过只在用户写错的时候会出现这种效果，不急。
		if (!just_skip_symbol(true, WordItem::Symbol::Endd)) {
			expect_word_i_faild("结束函数定义", ";");
			return;
		}
	} else log::debug_log("fn[{}] def finish", string(fn_name.value()));

	table.items.push_back({ .value = ::std::move(fn_def) });
}

void Tree::build_仙言(Table &table) {
	for (i = 0; i < words.size();) {
		visit_word_i__start();
		if_T_is(WordItem::Symbol) {
			if (item == WordItem::Symbol::Cmdd) {
				i++;
				build_命令(item, table);
			} else {
				expect_word_i_faild("分析仙言", "command|define");
				just_skip_line();
				i++;
			}
		}
		elif_T_is(WordItem::Keyword) {
			if (item == WordItem::Keyword::Fn) {
				i++;
				build_函数定义(table);
			} else {
				expect_word_i_faild("分析仙言", "command|define");
				just_skip_line();
				i++;
			}
		}
		elif_T_is(WordItem::Whitespace) { i++; }
		else {
			expect_word_i_faild("分析仙言", "command|define");
			just_skip_line();
			i++;
		}
		visit_word_i__end();
	}
}

Table Tree::build() {
	Table table;

	log::debug_log("start building tree...");
	build_仙言(table);
	log::debug_log("end building tree!");

	return table;
}

table::TreeTable build_tree(table::WordTable const &words) {
	return Tree(words).build();
}

}  //namespace xn::tree
