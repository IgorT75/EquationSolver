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

		[[nodiscard]] std::vector<defs::lex_wrapper> solve() const {
			std::stack<defs::lex_wrapper> st;
			std::vector<defs::lex_wrapper> out;
			for (const auto & lw : _vec) {
				if (lw.lex_type == defs::lex::number || lw.lex_type == defs::lex::constant)
					out.push_back(lw);
				else if (lw.lex_type == defs::lex::function)
					st.push(lw);
				else if (defs::is_operator(lw.lex_type)) {
					while (!st.empty() && defs::is_operator(st.top().lex_type) &&
						(std::get<1>(defs::op_props[lw.lex_type])  < std::get<1>(defs::op_props[st.top().lex_type]) || 
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
	};
}
