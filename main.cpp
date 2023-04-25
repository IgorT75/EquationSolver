#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <variant>

//#include <boost/function.hpp>

#include <iostream>

#include "tokenizer.h"
#include "shunting_yard.h"

using namespace defs;

int main()
{
	std::string eq { "2+3*4^2" };
	eq = "log(+82.3)+7^sin(+3.9^2)";
	//eq = "Min(33, 4)";
	//eq = "5+3>8";
	//eq = "2^If(5 == 8 | 6 > 5, 3, 4)";
	//eq = "sin(30)";
	//eq = "sin(60-min(3,4))";
	std::map<std::string, double> vars { { "a", 2.0 }, { "b", 3.0 }, { "c", 4.0 } };
	while (!eq.empty()) {
		std::getline(std::cin, eq);
		if (eq == "q") break;

		const solver::tokenizer parser{ eq, { "a", "b", "c" }};
		std::vector<defs::lex_wrapper> vec_lex = parser.parse();
		if (vec_lex.empty() || vec_lex.back().lex_type == lex::error) {
			const error err = vec_lex.back().data.index() == 4 ? std::get<4>(vec_lex.back().data) : error::unknown;
			const std::string s_err = error_str[err];
			printf("Cannot parse: %s due to %s error\n\n", eq.c_str(), s_err.c_str());
			continue;
		}

		// remove end
		vec_lex.erase(--vec_lex.end());

		const auto algo = solver::shunting_yard { vec_lex, vars };
		if (const auto res = algo.solve(); res.index() == 0) {
			const double d = std::get<double>(res);
			printf("%s = %f\n\n", eq.c_str(), d);
		}
		else if (res.index() == 1) {
			const std::string& s = std::get<bool>(res) ? "true" : "false";
			printf("%s = %s\n\n", eq.c_str(), s.c_str());
		}
		else {
			const auto& err = std::get<2>(res);
			const std::string& s_err = error_str[err];
			printf("%s: %s\n\n", eq.c_str(), s_err.c_str());
		}
	}
}

