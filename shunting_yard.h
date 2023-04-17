#pragma once
#include <vector>
#include <string>
#include <optional>
#include <stack>

#include "maps.h"

namespace solver
{
	class shunting_yard
	{
		std::vector<defs::lex_wrapper> _vec;

	public:
		shunting_yard(std::vector<defs::lex_wrapper> vec) : _vec(std::move(vec)) {}

		[[nodiscard]] std::vector<defs::lex_wrapper> run() const {
			std::stack<defs::lex_wrapper> st;
			std::vector<defs::lex_wrapper> out;
			for (const auto& lw : _vec) {
				if (lw.lex_type == defs::lex::number || lw.lex_type == defs::lex::constant)
					out.push_back(lw);
				else if (lw.lex_type == defs::lex::function)
					st.push(lw);
				else if (defs::is_operator(lw.lex_type)) {
					while (!st.empty() && defs::is_operator(st.top().lex_type) &&
						(std::get<1>(defs::op_props[lw.lex_type]) < std::get<1>(defs::op_props[st.top().lex_type]) ||
							(std::get<1>(defs::op_props[lw.lex_type]) == std::get<1>(defs::op_props[st.top().lex_type]) &&
								std::get<2>(defs::op_props[lw.lex_type]) == defs::associativity::left))) {
						out.push_back(st.top());
						st.pop();
					}
					st.push(lw);
				}
				else if (lw.lex_type == defs::lex::comma) {
					while (!st.empty() && st.top().lex_type != defs::lex::lb) {
						out.push_back(st.top());
						st.pop();
					}
				}
				else if (lw.lex_type == defs::lex::lb) {
					st.push(lw);
				}
				else if (lw.lex_type == defs::lex::rb) {
					while (!st.empty() && st.top().lex_type != defs::lex::lb) {
						out.push_back(st.top());
						st.pop();
					}
					st.pop();

					if (!st.empty()) {
						if (st.top().lex_type == defs::lex::function) {
							out.push_back(st.top());
							st.pop();
						}
					}
				}
			}

			while (!st.empty()) {
				out.push_back(st.top());
				st.pop();
			}

			return out;
		}

		[[nodiscard]] std::variant<double, bool, error> rpn_compute(const std::vector<defs::Term>& terms) const {
			auto it = terms.begin();
			std::stack<std::variant<double, bool>> st;
			while (it != terms.end())// || st.size() != 1)
			{
				if (it->index() == 0) {
					st.push(std::get<double>(*it));
				}
				else if (it->index() == 1) {
					st.push(std::get<bool>(*it));
				}
				else {
					std::vector<std::variant<double, bool>> vec;
					size_t i = 0;
					const size_t nArgs = defs::term_args_map[it->index()];
					while (!st.empty() && i++ < nArgs) {
						vec.push_back(st.top());
						st.pop();
					}

					if (vec.size() != nArgs)
						return error::wrong_num_args;

					std::variant<double, bool> ret;
					switch (it->index()) {
					case 2: { // double from double
						if (vec[0].index() != 0)
							return error::wrong_type;

						auto op2 = std::get<2>(*it);
						auto f2 = op2.executor();
						ret = f2(std::get<double>(vec[0]));
						break;
					}
					case 3: { // double from two doubles
						if (vec[0].index() != 0 || vec[1].index() != 0)
							return error::wrong_type;

						auto op3 = std::get<3>(*it);
						auto f3 = op3.executor();
						ret = f3(std::get<double>(vec[1]), std::get<double>(vec[0]));
						break;
					}
					case 4: { // If function: returns double from bool and two doubles
						if (vec[0].index() != 0 || vec[1].index() != 0 || vec[2].index() != 1)
							return error::wrong_type;

						auto op4 = std::get<4>(*it);
						auto f4 = op4.executor();
						ret = f4(std::get<bool>(vec[2]), std::get<double>(vec[1]), std::get<double>(vec[0]));
						break;
					}
					case 5: { // returns boolean from two doubles
						if (vec[0].index() != 0 || vec[1].index() != 0)
							return error::wrong_type;

						auto op5 = std::get<5>(*it);
						auto f5 = op5.executor();
						ret = f5(std::get<double>(vec[1]), std::get<double>(vec[0]));
						break;
					}
					case 6: { // returns boolean from two booleans
						if (vec[0].index() != 1 || vec[1].index() != 1)
							return error::wrong_type;

						auto op6 = std::get<6>(*it);
						auto f6 = op6.executor();
						ret = f6(std::get<bool>(vec[1]), std::get<bool>(vec[0]));
						break;
					}
					case 7: { // returns const double
						auto op7 = std::get<7>(*it);
						auto f7 = op7.executor();
						ret = f7();
						break;
					}
					}

					st.push(ret);
				}
				++it;
			}

			if (st.size() == 1) {
				const auto& top = st.top();
				if (top.index() == 0)
					return std::get<0>(top);

				return std::get<1>(top);
			}
			return std::numeric_limits<double>::quiet_NaN();
		}

		[[nodiscard]] std::variant<double, bool, error> solve() const {

			// run shunting yard algorithm
			std::vector<lex_wrapper> vec = run();

			// transform result
			std::vector<defs::Term> vec_term;
			std::ranges::for_each(vec, [&vec_term](const defs::lex_wrapper& lw) {
				if (lw.lex_type == lex::number)
					vec_term.emplace_back(std::get<double>(lw.data));
				else if (lw.lex_type == lex::boolean)
					vec_term.emplace_back(std::get<bool>(lw.data));
				else if (defs::is_operator(lw.lex_type)) {
					vec_term.emplace_back(defs::op_map[lw.lex_type]);
				}
				else if (lw.lex_type == lex::function) {
					vec_term.emplace_back(defs::func_map[std::get<std::string>(lw.data)]);
				}
				else if (lw.lex_type == lex::constant) {
					vec_term.emplace_back(defs::const_map[std::get<std::string>(lw.data)]);
				}
			});

			// run rpn algo
			const std::variant<double, bool, error> res = rpn_compute(vec_term);
			return res;
		}
	};
}
