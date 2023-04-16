#pragma once
#include <boost/function.hpp>

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

	template<typename R, typename... T>
	class op_general
	{
		lex _lex;
		int _nArgs;
		boost::function<R(T...)> _f;
	public:
		op_general(lex lex, int nArgs, boost::function<R(T...)> f) :
			_lex(lex), _nArgs(nArgs), _f(std::move(f)) {  }

		[[nodiscard]] int num_args() const { return _nArgs; };
		boost::function<R(T...)> executor() const { return _f; }
	};

	class op_defs {
	public:
		inline static op_general<double> Pi{ lex::function, 0, boost::function<double()>([] { return 3.1415926535897932384626433832795028841968; }) };

		inline static op_general<double, double, double> Add{ lex::plus, 2, boost::function<double(double, double)>([](double a, double b) { return a + b; }) };
		inline static op_general<double, double, double> Sub{ lex::minus, 2, boost::function<double(double, double)>([](double a, double b) { return a - b; }) };
		inline static op_general<double, double, double> Mul{ lex::multiply, 2, boost::function<double(double, double)>([](double a, double b) { return a * b; }) };
		inline static op_general<double, double, double> Div{ lex::divide, 2, boost::function<double(double, double)>([](double a, double b) { return a / b; }) };
		inline static op_general<double, double, double> Pow{ lex::power, 2, boost::function<double(double, double)>([](double a, double b) { return std::pow(a, b); }) };
		inline static op_general<double, double, double> Max{ lex::function, 2, boost::function<double(double, double)>([](double a, double b) { return std::max(a, b); }) };
		inline static op_general<double, double, double> Min{ lex::function, 2, boost::function<double(double, double)>([](double a, double b) { return std::min(a, b); }) };
		inline static op_general<double, double, double> Atan2{ lex::function, 2, boost::function<double(double, double)>([](double a, double b) { return std::atan2(a, b); }) };

		inline static op_general<double, double> Neg{ lex::function, 1, boost::function<double(double)>([](double a) { return -a; }) };
		inline static op_general<double, double> Sin{ lex::function, 1, boost::function<double(double)>([](double a) { return std::sin(a); }) };
		inline static op_general<double, double> Cos{ lex::function, 1, boost::function<double(double)>([](double a) { return std::cos(a); }) };
		inline static op_general<double, double> Tan{ lex::function, 1, boost::function<double(double)>([](double a) { return std::tan(a); }) };
		inline static op_general<double, double> Ctn{ lex::function, 1, boost::function<double(double)>([](double a) { return 1.0 / std::tan(a); }) };
		inline static op_general<double, double> Atan{ lex::function, 1, boost::function<double(double)>([](double a) { return std::atan(a); }) };
		inline static op_general<double, double> Sign{ lex::function, 1, boost::function<double(double)>([](double a) { return a > 0.0 ? 1.0 : (a < 0.0 ? -1.0 : 0.0); }) };
		inline static op_general<double, double> Exp{ lex::function, 1, boost::function<double(double)>([](double a) { return std::exp(a); }) };
		inline static op_general<double, double> Ln{ lex::function, 1, boost::function<double(double)>([](double a) { return std::log(a); }) };
		inline static op_general<double, double> Log{ lex::function, 1, boost::function<double(double)>([](double a) { return std::log10(a); }) };
		inline static op_general<double, double> Floor{ lex::function, 1, boost::function<double(double)>([](double a) { return std::floor(a); }) };
		inline static op_general<double, double> Ceil{ lex::function, 1, boost::function<double(double)>([](double a) { return std::ceil(a); }) };
		inline static op_general<double, double> Round{ lex::function, 1, boost::function<double(double)>([](double a) { return std::round(a); }) };
		inline static op_general<double, double> Abs{ lex::function, 1, boost::function<double(double)>([](double a) { return std::abs(a); }) };

		inline static op_general<double, bool, double, double> If{ lex::function, 3, boost::function<double(bool, double, double)>([](bool cond, double a, double b) { return cond ? a : b; }) };

		inline static op_general<bool, double, double> Equal{ lex::equal, 2, boost::function<bool(double, double)>([](double a, double b) { return a == b; }) };
		inline static op_general<bool, double, double> NotEqual{ lex::not_equal, 2, boost::function<bool(double, double)>([](double a, double b) { return a != b; }) };
		inline static op_general<bool, double, double> Less{ lex::less, 2, boost::function<bool(double, double)>([](double a, double b) { return a < b; }) };
		inline static op_general<bool, double, double> LessOrEqual{ lex::less_equal, 2, boost::function<bool(double, double)>([](double a, double b) { return a <= b; }) };
		inline static op_general<bool, double, double> More{ lex::more, 2, boost::function<bool(double, double)>([](double a, double b) { return a > b; }) };
		inline static op_general<bool, double, double> MoreOrEqual{ lex::more_equal, 2, boost::function<bool(double, double)>([](double a, double b) { return a >= b; }) };

		inline static op_general<bool, bool, bool> LogicalAnd{ lex::logic_and, 2, boost::function<bool(bool, bool)>([](bool a, bool b) { return a && b; }) };
		inline static op_general<bool, bool, bool> LogicalOr{ lex::logic_and, 2, boost::function<bool(bool, bool)>([](bool a, bool b) { return a || b; }) };
		inline static op_general<bool, bool, bool> LogicalXor{ lex::logic_and, 2, boost::function<bool(bool, bool)>([](bool a, bool b) { return a ^ b; }) };
	};

	inline auto is_operator(lex l) -> bool {
		static auto v = { lex::plus, lex::minus, lex::unary_minus, lex::multiply, lex::divide, lex::power, lex::less,
						  lex::less_equal, lex::more, lex::more_equal, lex::equal, lex::logic_or, lex::logic_and, lex::logic_xor };
		return std::ranges::find(v, l) != v.end();
	}
}