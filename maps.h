#pragma once
#include <map>
#include <utility>
#include <vector>
#include <string>

#include "operators.h"
using namespace defs;  // NOLINT(clang-diagnostic-header-hygiene)

namespace lexical
{
	inline auto is_operator(lex l) -> bool {
		static auto v = { lex::plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power, lex::less,
						  lex::less_equal, lex::more, lex::more_equal, lex::equal, lex::logic_or, lex::logic_and, lex::logic_xor };
		return std::ranges::find(v, l) != v.end();
	}

	// lex --> #args, precedence, associativity
	inline static std::map<lex, std::tuple<int, int, associativity>> op_props{
		{ lex::plus,		{ 2, 3, associativity::left } },
		{ lex::minus,		{ 2, 3, associativity::left } },
		{ lex::unary_minus, { 2, 3, associativity::left } },
		{ lex::multiply,	{ 2, 4, associativity::left } },
		{ lex::divide,		{ 2, 4, associativity::left } },
		{ lex::logic_or,	{ 2, 1, associativity::left } },
		{ lex::logic_and,	{ 2, 1, associativity::left } },
		{ lex::logic_xor,	{ 2, 1, associativity::left } },
		{ lex::less,		{ 2, 2, associativity::left } },
		{ lex::less_equal,	{ 2, 2, associativity::left } },
		{ lex::more,		{ 2, 2, associativity::left } },
		{ lex::more_equal,	{ 2, 2, associativity::left } },
		{ lex::equal,		{ 2, 2, associativity::left } },
		{ lex::power,		{ 2, 5, associativity::right } }
	};

	// value is a list of lexes that can happen before key
	inline std::map<lex, std::vector<lex>> lex_order {
		{ lex::begin,       { } },
		{ lex::lb,          { lex::begin, lex::lb, lex::comma, lex::plus, lex::unary_plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power, lex::function, lex::less, lex::less_equal, lex::more, lex::more_equal, lex::equal, lex::logic_or, lex::logic_and, lex::logic_xor } }, // (
		{ lex::rb,          { lex::rb, lex::variable, lex::number, lex::constant } }, // )
		{ lex::comma,       { lex::rb, lex::number, lex::variable, lex::constant } }, //,
		{ lex::plus,        { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::unary_plus,  { lex::begin, lex::lb, lex::number, lex::variable, lex::constant } },
		{ lex::minus,       { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::unary_minus, { lex::begin, lex::lb, lex::number, lex::variable, lex::constant } },
		{ lex::multiply,    { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::divide,      { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::power,       { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::less,        { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::less_equal,  { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::more,        { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::more_equal,  { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::equal,       { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::logic_or,    { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::logic_and,   { lex::rb, lex::number, lex::variable, lex::constant } },
		{ lex::number,      { lex::begin, lex::lb, lex::comma, lex::plus, lex::unary_plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power } },
		{ lex::constant,	{ lex::begin, lex::lb, lex::comma, lex::plus, lex::unary_plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power } },
		{ lex::variable,    { lex::begin, lex::plus, lex::unary_plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power } },
		{ lex::function,    { lex::begin, lex::lb, lex::plus, lex::unary_plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power } },
		{ lex::end,         { lex::rb, lex::number, lex::variable, lex::constant } }
	};
	
	class lex_operators {
	public:
		inline static const std::string l_brace = "(";
		inline static const std::string r_brace = ")";
		inline static const std::string comma = ",";
		inline static const std::string plus = "+";
		inline static const std::string minus = "-";
		inline static const std::string multiply = "*";
		inline static const std::string divide = "/";
		inline static const std::string power = "^";
		inline static const std::string less = "<";
		inline static const std::string less_equal = "<=";
		inline static const std::string more = ">";
		inline static const std::string more_equal = ">=";
		inline static const std::string equal = "==";
		inline static const std::string logic_or = "|";
		inline static const std::string logic_and = "&";

		inline static const std::vector<std::string> names {
			less_equal, more_equal, equal, r_brace, l_brace, comma, plus, minus, multiply, divide, power, less, more, logic_or, logic_and
		};
	};

	inline static std::map<std::string, std::vector<lex>> lex_oper_map {
		{ lex_operators::r_brace,    { lex::rb } },
		{ lex_operators::l_brace,    { lex::lb } },
		{ lex_operators::comma,      { lex::comma } },
		{ lex_operators::plus,       { lex::plus, lex::unary_plus } },
		{ lex_operators::minus,      { lex::minus, lex::unary_minus } },
		{ lex_operators::multiply,   { lex::multiply } },
		{ lex_operators::divide,     { lex::divide } },
		{ lex_operators::power,      { lex::power } },
		{ lex_operators::less,       { lex::less } },
		{ lex_operators::less_equal, { lex::less_equal } },
		{ lex_operators::more,       { lex::more } },
		{ lex_operators::more_equal, { lex::more_equal } },
		{ lex_operators::equal,      { lex::equal } },
		{ lex_operators::logic_or,   { lex::logic_or } },
		{ lex_operators::logic_and,  { lex::logic_and } },
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////// function as a special case of operator ///////////////////////////////////
	class lex_functions {
	public:
		inline static const std::string sin = "sin";
		inline static const std::string cos = "cos";
		inline static const std::string tan = "tan";
		inline static const std::string abs = "abs";
		inline static const std::string sign = "sign";
		inline static const std::string ctn = "ctn";
		inline static const std::string atan = "atan";
		inline static const std::string atan2 = "atan2";
		inline static const std::string min = "min";
		inline static const std::string max = "max";
		inline static const std::string iff = "if";
		inline static const std::string intt = "int";
		inline static const std::string floor = "floor";
		inline static const std::string ceil = "ceil";
		inline static const std::string round = "round";
		inline static const std::string log = "log";
		inline static const std::string log10 = "log10";
		inline static const std::string ln = "ln";
		inline static const std::string pow = "pow";
		inline static const std::string exp = "exp";

		inline static const std::vector<std::string> names {
			sin, cos, tan, abs, sign, ctn, atan2, atan, min, max, iff, intt, floor, ceil, round, log10, log, ln, exp
		};
	};

	// consts as a special case of functions
	class lex_consts {
	public:
		inline static const std::string pi = "pi";
		inline static const std::vector<std::string> names { pi };
	};

	typedef std::variant<double, bool, op_general<double, double>, op_general<double, double, double>, op_general<double, bool, double, double>,
			op_general<bool, double, double>, op_general<bool, bool, bool>, op_general<double>> Term;

	// index of variable in Term to # of arguments
	inline static std::map<size_t, size_t> term_args_map {
		{ 2, 1 }, { 3, 2 }, { 4, 3 }, { 5, 2 }, { 6, 2 }, { 7, 0 }
	};

	// map of function to action
	inline static std::map<const std::string, Term> func_map {
		{ lex_functions::sin,   op_defs::Sin },
		{ lex_functions::cos,   op_defs::Cos },
		{ lex_functions::tan,   op_defs::Tan },
		{ lex_functions::atan,  op_defs::Atan },
		{ lex_functions::atan2, op_defs::Atan2 },
		{ lex_functions::ctn,   op_defs::Ctn },
		{ lex_functions::min,   op_defs::Min },
		{ lex_functions::max,   op_defs::Max },
		{ lex_functions::iff,   op_defs::If },
		{ lex_functions::floor, op_defs::Floor },
		{ lex_functions::ceil,  op_defs::Ceil },
		{ lex_functions::round, op_defs::Round },
		{ lex_functions::log,   op_defs::Log },
		{ lex_functions::log10, op_defs::Log },
		{ lex_functions::ln,    op_defs::Ln },
		{ lex_functions::exp,   op_defs::Exp },
		{ lex_functions::sign,  op_defs::Sign },
	};

	// map of operator to action
	inline static std::map<lex, Term> op_map {
		{ lex::plus,          op_defs::Add },
		{ lex::minus,         op_defs::Sub },
		{ lex::unary_minus,   op_defs::Neg },
		{ lex::multiply,      op_defs::Mul },
		{ lex::divide,        op_defs::Div },
		{ lex::power,         op_defs::Pow },
		{ lex::equal,         op_defs::Equal },
		{ lex::less,          op_defs::Less },
		{ lex::less_equal,    op_defs::LessOrEqual },
		{ lex::more,          op_defs::More },
		{ lex::more_equal,    op_defs::MoreOrEqual },
		{ lex::logic_and,     op_defs::LogicalAnd },
		{ lex::logic_or,      op_defs::LogicalOr },
		{ lex::logic_xor,     op_defs::LogicalXor }
	};

	// map of consts to action
	inline static std::map<const std::string, Term> const_map {
		{ lex_consts::pi,    op_defs::Pi }
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct lex_wrapper {
		lex_wrapper(lex l) : lex_type(l) {}
		lex_wrapper(lex l, std::variant<std::monostate, double, bool, std::string> d) : lex_wrapper(l) {
			data = std::move(d);
		}

		bool operator==(const lex_wrapper& other) const {
			return this->lex_type == other.lex_type && this->data == other.data;
		}

		lex lex_type;
		std::variant<std::monostate, double, bool, std::string> data;
	};

	inline static lex_wrapper w_unary_plus { lex::unary_plus };
}
