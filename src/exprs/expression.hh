#pragma once

#include <any>
#include <atomic>
#include <memory>
#include <vector>

namespace xn {
class Context;

namespace exprs {
	class Expression;
	using Node = ::std::shared_ptr<Expression>;
	using Ast  = ::std::vector<Node>;
}

class Namespace {
  private:
	Context &ctx;
	bool     is_initialized = false;

	::std::atomic_uint32_t fn_counter = 0;
	exprs::Ast             tree;

	::std::unique_ptr<::std::any> module;
	char                          op;
};
}
