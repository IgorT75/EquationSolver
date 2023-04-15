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
		std::vector<lexical::lex_wrapper> _vec;

	public:
		shunting_yard(std::vector<lexical::lex_wrapper> vec) : _vec(std::move(vec)) {}

		[[nodiscard]] std::vector<lexical::lex_wrapper> solve() const {
			std::stack<lexical::lex_wrapper> st;
			std::vector<lexical::lex_wrapper> out;
			for (const auto & lw : _vec) {
				if (lw.lex_type == lex::number || lw.lex_type == lex::constant)
					out.push_back(lw);
				else if (lw.lex_type == lex::function)
					st.push(lw);
				else if (lexical::is_operator(lw.lex_type)) {
					while (!st.empty() && lexical::is_operator(st.top().lex_type) &&
						(std::get<1>(lexical::op_props[lw.lex_type])  < std::get<1>(lexical::op_props[st.top().lex_type]) || 
						(std::get<1>(lexical::op_props[lw.lex_type]) == std::get<1>(lexical::op_props[st.top().lex_type]) && 
						 std::get<2>(lexical::op_props[lw.lex_type]) == associativity::left))) {
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
			}

			while (!st.empty()) {
				out.push_back(st.top());
				st.pop();
			}

			return out;
		}
	};
}
