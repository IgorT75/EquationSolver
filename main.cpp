#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <variant>

//#include <boost/function.hpp>

#include "lex_parser.h"
#include "rpn_solver.h"
#include "shunting_yard.h"

using namespace defs;

int main()
{
	std::string eq { "2+3*4^2" };
	//eq = "log(+82.3)+7^sin(+3.9^2)";
	eq = "if(3>9 | 99-8 < 99+3, 3, 23) + 7";
	//eq = "7+3>5";
	const lexical::lex_parser parser{ eq };
	//parser = "sin(60-min(3,4))";
	std::vector<lexical::lex_wrapper> vec_lex = parser.parse();
	if (vec_lex.empty() || vec_lex.back().lex_type == lex::error) {
		printf("Cannot parse: %s\n\n", eq.c_str());
		return -1;
	}

	// remove unary_pluses
	const auto itr = std::ranges::remove_if(vec_lex, [](const auto& lw) { return lw.lex_type == lex::unary_plus; }).begin();
	vec_lex.erase(itr, vec_lex.end());

	vec_lex.erase(vec_lex.begin());
	vec_lex.erase(--vec_lex.end());

	const auto algo = solver::shunting_yard { vec_lex };
	std::vector<lexical::lex_wrapper> vec = algo.solve();

	std::vector<lexical::Term> vec_term;
	std::ranges::for_each(vec, [&vec_term](const lexical::lex_wrapper& lw) {
		if (lw.lex_type == lex::number)
			vec_term.emplace_back(std::get<double>(lw.data));
		else if (lw.lex_type == lex::boolean)
			vec_term.emplace_back(std::get<bool>(lw.data));
		else if (lexical::is_operator(lw.lex_type)) {
			vec_term.emplace_back(lexical::op_map[lw.lex_type]);
		}
		else if (lw.lex_type == lex::function) {
			vec_term.emplace_back(lexical::func_map[std::get<std::string>(lw.data)]);
		}
		else if (lw.lex_type == lex::constant) {
			vec_term.emplace_back(lexical::const_map[std::get<std::string>(lw.data)]);
		}
	});

	const solver::rpn_solver rpn { vec_term };
	const std::variant<double, bool> res = rpn.solve();
	if (res.index() == 0) {
		const double d = std::get<double>(res);
		printf("%s = %f\n\n", eq.c_str(), d);
	}
	else {
		const std::string s = std::get<bool>(res) ? "true" : "false";
		printf("%s = %s\n\n", eq.c_str(), s.c_str());
	}
}

