#pragma once
#include "common_types.h"

namespace defs
{
	enum class lex
	{
		begin,
		lb,
		rb,
		comma,
		plus,
		unary_plus,
		minus,
		unary_minus,
		multiply,
		divide,
		power,
		less,
		less_equal,
		more,
		more_equal,
		equal,
		not_equal,
		logic_or,
		logic_and,
		logic_xor,
		number,
		constant,
		boolean,
		variable,
		function,
		end,
		error
	};

	enum class associativity { left, right, none };

	class op_defs {
	public:
		inline static num_empty_t pi_f = [] { return cnum_t { 3.1415926535897932384626433832795028841968 }; };

		inline static num_2num_t add_f = [](cnum_t a, cnum_t b) { return a + b; };
		inline static num_2num_t sub_f = [](cnum_t a, cnum_t b) { return a - b; };
		inline static num_2num_t mul_f = [](cnum_t a, cnum_t b) { return a * b; };
		inline static num_2num_t div_f = [](cnum_t a, cnum_t b) { return a / b; };
		inline static num_2num_t pow_f = [](cnum_t a, cnum_t b) { return std::pow(a, b); };
		inline static num_2num_t min_f = [](cnum_t a, cnum_t b) { return std::min(a, b); };
		inline static num_2num_t max_f = [](cnum_t a, cnum_t b) { return std::max(a, b); };
		inline static num_2num_t atan2_f = [](cnum_t a, cnum_t b) { return std::atan2(a, b); };

		inline static num_1num_t neg_f   = [](cnum_t a) { return -a; };
		inline static num_1num_t sin_f   = [](cnum_t a) { return std::sin(a); };
		inline static num_1num_t cos_f   = [](cnum_t a) { return std::cos(a); };
		inline static num_1num_t tan_f   = [](cnum_t a) { return std::tan(a); };
		inline static num_1num_t ctn_f   = [](cnum_t a) { return 1.0/std::tan(a); };
		inline static num_1num_t atan_f  = [](cnum_t a) { return std::atan(a); };
		inline static num_1num_t sing_f  = [](cnum_t a) { return a > 0.0 ? 1.0 : (a < 0.0 ? -1.0 : 0.0); };
		inline static num_1num_t exp_f   = [](cnum_t a) { return std::exp(a); };
		inline static num_1num_t ln_f    = [](cnum_t a) { return std::log(a); };
		inline static num_1num_t log_f   = [](cnum_t a) { return std::log10(a); };
		inline static num_1num_t floor_f = [](cnum_t a) { return std::floor(a); };
		inline static num_1num_t ceil_f  = [](cnum_t a) { return std::ceil(a); };
		inline static num_1num_t round_f = [](cnum_t a) { return std::round(a); };
		inline static num_1num_t abs_f   = [](cnum_t a) { return std::abs(a); };

		inline static num_1bool_2num_t if_f = [](cbool_t cond, cnum_t a, cnum_t b) { return cond ? a : b; };

		inline static bool_2num_t equal_f      = [](cnum_t a, cnum_t b) { return a == b; };
		inline static bool_2num_t not_equal_f  = [](cnum_t a, cnum_t b) { return a != b; };
		inline static bool_2num_t less_f       = [](cnum_t a, cnum_t b) { return a < b; };
		inline static bool_2num_t less_equal_f = [](cnum_t a, cnum_t b) { return a <= b; };
		inline static bool_2num_t more_f       = [](cnum_t a, cnum_t b) { return a > b; };
		inline static bool_2num_t more_equal_f = [](cnum_t a, cnum_t b) { return a >= b; };

		inline static bool_2bool_t logic_and_f = [](cbool_t a, cbool_t b) { return a && b; };
		inline static bool_2bool_t logic_or_f  = [](bool_t a, cbool_t b) { return a || b; };
		inline static bool_2bool_t logic_xor_f = [](cbool_t a, cbool_t b) { return a != b; };
	};

	inline auto is_operator(lex l) -> bool {
		static auto v = { lex::plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power, lex::less,
						  lex::less_equal, lex::more, lex::more_equal, lex::equal, lex::logic_or, lex::logic_and, lex::logic_xor };
		return std::ranges::find(v, l) != v.end();
	}
}
