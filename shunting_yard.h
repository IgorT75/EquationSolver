#pragma once
#include <vector>
#include <string>
#include <optional>
#include <stack>
#include <algorithm>
//#include <complex>

#include "maps.h"

namespace solver
{
	//template<typename Num, typename Bln>
	class shunting_yard
	{
		std::vector<lex_wrapper> _vec;
		std::map<std::string, num_t> _variables;

	public:
		shunting_yard(std::vector<lex_wrapper> vec, std::map<std::string, num_t> vars) :
	    _vec(std::move(vec)), _variables(std::move(vars)) {}

		[[nodiscard]] std::vector<lex_wrapper> run() const {
			std::stack<lex_wrapper> st;
			std::vector<lex_wrapper> out;
			size_t idx = 0;
			for (const auto& lw : _vec) {
				if (lw.lex_type == lex::number || lw.lex_type == lex::constant || lw.lex_type == lex::variable)
					out.push_back(lw);
				else if (lw.lex_type == lex::function)
					st.push(lw);
				else if (is_operator(lw.lex_type)) {
					while (!st.empty() && is_operator(st.top().lex_type) &&
						(std::get<1>(op_props[lw.lex_type]) < std::get<1>(op_props[st.top().lex_type]) ||
						(std::get<1>(op_props[lw.lex_type]) == std::get<1>(op_props[st.top().lex_type]) &&
						 std::get<2>(op_props[lw.lex_type]) == associativity::left))) {
						out.push_back(st.top());
						st.pop();
					}
					st.push(lw);
				}
				else if (lw.lex_type == lex::comma) {
					while (!st.empty() && st.top().lex_type != lex::lb) {
						out.push_back(st.top());
						st.pop();
					}
				}
				else if (lw.lex_type == lex::lb) {
					st.push(lw);
				}
				else if (lw.lex_type == lex::rb) {
					while (!st.empty() && st.top().lex_type != lex::lb) {
						out.push_back(st.top());
						st.pop();
					}
					st.pop();

					if (!st.empty()) {
						if (st.top().lex_type == lex::function) {
							out.push_back(st.top());
							st.pop();
						}
					}
				}
				++idx;
			}

			while (!st.empty()) {
				out.push_back(st.top());
				st.pop();
			}

			return out;
		}

		[[nodiscard]] std::variant<num_t, bool, error> rpn_compute(const std::vector<lex_wrapper>& vec_lw) const {
			// transform result
			std::vector<Term> terms;
			std::ranges::for_each(vec_lw, [&](const lex_wrapper& lw) {
				if (lw.lex_type == lex::number)
					terms.emplace_back(std::get<num_t>(lw.data));
				if (lw.lex_type == lex::variable)
					terms.emplace_back(_variables.at(std::get<std::string>(lw.data)));
				else if (lw.lex_type == lex::boolean)
					terms.emplace_back(std::get<bool_t>(lw.data));
				else if (is_operator(lw.lex_type)) {
					terms.emplace_back(op_map.at(lw.lex_type));
				}
				else if (lw.lex_type == lex::function) {
					terms.emplace_back(func_map.at(std::get<std::string>(lw.data)));
				}
				else if (lw.lex_type == lex::constant) {
					terms.emplace_back(const_map.at(std::get<std::string>(lw.data)));
				}
			});

			auto it = terms.begin();
			std::stack<std::variant<num_t, bool>> st;
			size_t idx = 0;
			while (it != terms.end())// || st.size() != 1)
			{
				if (it->index() == 0) {
					st.push(std::get<num_t>(*it));
				}
				else if (it->index() == 1) {
					st.push(std::get<bool>(*it));
				}
				else {
					std::vector<std::variant<num_t, bool>> vec;
					size_t i = 0;
					const size_t n_args = vec_lw[idx].n_args;
					while (!st.empty() && i++ < n_args) {
						vec.push_back(st.top());
						st.pop();
					}

					if (vec.size() != n_args)
						return error::wrong_args_count;

					std::variant<num_t, bool> ret;
					switch (it->index()) {
						case 2: { // double from double
							if (vec[0].index() != 0)
								return error::wrong_type;

							const auto& op2 = std::get<2>(*it);
							ret = op2(std::get<num_t>(vec[0]));
							break;
						}
						case 3: { // double from two doubles
							if (std::ranges::any_of(vec, [](const std::variant<num_t, bool>& v) { return v.index() != 0; }))
								return error::wrong_type;

							const auto& op3 = std::get<3>(*it);
              if (vec.size() == 2)
							  ret = op3(std::get<num_t>(vec[1]), std::get<num_t>(vec[0]));
							else {
								// got to be min or max
                auto vec_it = vec.begin();
								ret = std::get<num_t>(*vec_it);
								while (++vec_it != vec.end()) {
									ret = op3(std::get<num_t>(ret), std::get<num_t>(*vec_it));
								}
							}
							break;
						}
						case 4: { // If function: returns double from bool and two doubles
							if (vec[0].index() != 0 || vec[1].index() != 0 || vec[2].index() != 1)
								return error::wrong_type;

							const auto& op4 = std::get<4>(*it);
							ret = op4(std::get<bool>(vec[2]), std::get<num_t>(vec[1]), std::get<num_t>(vec[0]));
							break;
						}
						case 5: { // returns boolean from two doubles
							if (vec[0].index() != 0 || vec[1].index() != 0)
								return error::wrong_type;

							const auto& op5 = std::get<5>(*it);
							ret = op5(std::get<num_t>(vec[1]), std::get<num_t>(vec[0]));
							break;
						}
						case 6: { // returns boolean from two booleans
							if (vec[0].index() != 1 || vec[1].index() != 1)
								return error::wrong_type;

							const auto& op6 = std::get<6>(*it);
							ret = op6(std::get<bool>(vec[1]), std::get<bool>(vec[0]));
							break;
						}
						case 7: { // returns const double
							const auto& op7 = std::get<7>(*it);
							ret = op7();
							break;
						}
					}

					st.push(ret);
				}
				++it;
				++idx;
			}

			if (st.size() == 1) {
				const auto& top = st.top();
				if (top.index() == 0)
					return std::get<0>(top);

				return std::get<1>(top);
			}
			return std::numeric_limits<num_t>::quiet_NaN();
		}

		[[nodiscard]] std::variant<num_t, bool_t, error> solve() const {

			// run shunting yard algorithm
      const std::vector<lex_wrapper> vec = run();

			// run rpn algo
			const std::variant<num_t, bool_t, error> res = rpn_compute(vec);
			return res;
		}
	};
}
