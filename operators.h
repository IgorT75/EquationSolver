#pragma once
#include <cmath>
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
		space,
		end,
		error
	};

	enum class associativity { left, right, none };

	class op_defs {
	public:
		inline static const num_empty_t pi_f = [] { return cnum_t { 3.1415926535897932384626433832795028841968 }; };

		inline static const num_2num_t add_f = [](cnum_t a, cnum_t b) { return a + b; };
		inline static const num_2num_t sub_f = [](cnum_t a, cnum_t b) { return a - b; };
		inline static const num_2num_t mul_f = [](cnum_t a, cnum_t b) { return a * b; };
		inline static const num_2num_t div_f = [](cnum_t a, cnum_t b) { return a / b; };
		inline static const num_2num_t pow_f = [](cnum_t a, cnum_t b) { return std::pow(a, b); };
		inline static const num_2num_t min_f = [](cnum_t a, cnum_t b) { return std::min(a, b); };
		inline static const num_2num_t max_f = [](cnum_t a, cnum_t b) { return std::max(a, b); };
		inline static const num_2num_t atan2_f = [](cnum_t a, cnum_t b) { return std::atan2(a, b); };

		inline static const num_1num_t neg_f   = [](cnum_t a) { return -a; };
		inline static const num_1num_t sin_f   = [](cnum_t a) { return std::sin(a); };
		inline static const num_1num_t cos_f   = [](cnum_t a) { return std::cos(a); };
		inline static const num_1num_t tan_f   = [](cnum_t a) { return std::tan(a); };
		inline static const num_1num_t ctn_f   = [](cnum_t a) { return cnum_t{1}/std::tan(a); };
		inline static const num_1num_t atan_f  = [](cnum_t a) { return std::atan(a); };
		inline static const num_1num_t sing_f  = [](cnum_t a) { return a > cnum_t{0} ? cnum_t{1} : (a < cnum_t{0} ? cnum_t{-1} : cnum_t{ 0 }); };
		inline static const num_1num_t exp_f   = [](cnum_t a) { return std::exp(a); };
		inline static const num_1num_t ln_f    = [](cnum_t a) { return std::log(a); };
		inline static const num_1num_t log_f   = [](cnum_t a) { return std::log10(a); };
		inline static const num_1num_t floor_f = [](cnum_t a) { return std::floor(a); };
		inline static const num_1num_t ceil_f  = [](cnum_t a) { return std::ceil(a); };
		inline static const num_1num_t round_f = [](cnum_t a) { return std::round(a); };
		inline static const num_1num_t abs_f   = [](cnum_t a) { return std::abs(a); };

		inline static const num_1bool_2num_t if_f = [](cbool_t cond, cnum_t a, cnum_t b) { return cond ? a : b; };

		inline static const bool_2num_t equal_f      = [](cnum_t a, cnum_t b) { return a == b; };
		inline static const bool_2num_t not_equal_f  = [](cnum_t a, cnum_t b) { return a != b; };
		inline static const bool_2num_t less_f       = [](cnum_t a, cnum_t b) { return a < b; };
		inline static const bool_2num_t less_equal_f = [](cnum_t a, cnum_t b) { return a <= b; };
		inline static const bool_2num_t more_f       = [](cnum_t a, cnum_t b) { return a > b; };
		inline static const bool_2num_t more_equal_f = [](cnum_t a, cnum_t b) { return a >= b; };

		inline static const bool_2bool_t logic_and_f = [](cbool_t a, cbool_t b) { return a && b; };
		inline static const bool_2bool_t logic_or_f  = [](cbool_t a, cbool_t b) { return a || b; };
		inline static const bool_2bool_t logic_xor_f = [](cbool_t a, cbool_t b) { return a != b; };
	};

  constexpr auto is_operator(const lex l) -> bool {
		return l == lex::plus || l == lex::minus || l == lex::unary_minus || l == lex::unary_plus || l == lex::multiply
	      || l == lex::divide || l == lex::power || l == lex::less || l == lex::less_equal || l == lex::more
	      || l == lex::more_equal || l == lex::equal || l == lex::logic_or || l == lex::logic_and || l == lex::logic_xor;
	}

  auto constexpr is_two_args_operator(const lex l) -> bool {
		return l == lex::plus || l == lex::minus || l == lex::multiply || l == lex::divide || l == lex::divide
			  || l == lex::power || l == lex::less || l == lex::less_equal || l == lex::more || l == lex::more_equal || l == lex::equal
			  || l == lex::logic_or || l == lex::logic_and || l == lex::logic_xor;
	}

	auto constexpr is_one_arg_operator(const lex l) -> bool { return l == lex::unary_minus || l == lex::unary_plus; }

  constexpr size_t args_count(const lex l) {
		if (is_two_args_operator(l)) return 2;
		if (is_one_arg_operator(l)) return 1;
		//if (l == lex::constant || l == lex::variable) return 0;
		return 0;
	}

	//inline auto is_operator0(const lex l) -> bool {
  //	static auto v = { lex::plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power, lex::less,
  //					  lex::less_equal, lex::more, lex::more_equal, lex::equal, lex::logic_or, lex::logic_and, lex::logic_xor };
  //	return std::ranges::find(v, l) != v.end();
  //}

  //inline auto is_two_args_operator0(const lex l) -> bool { return is_operator0(l) && l != lex::unary_minus; }
}
