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
					if (!st.empty()) {
						lexical::lex_wrapper& top = st.top();
						while (lexical::is_operator(top.lex_type) &&
							(std::get<1>(lexical::op_props[lw.lex_type])  < std::get<1>(lexical::op_props[top.lex_type]) || 
							(std::get<1>(lexical::op_props[lw.lex_type]) == std::get<1>(lexical::op_props[top.lex_type]) && 
							 std::get<2>(lexical::op_props[lw.lex_type]) == associativity::left))) {
							out.push_back(top);
							st.pop();
							if (st.empty()) break;
							top = st.top();
						}
					}
					st.push(lw);
				}
				else if (lw.lex_type == lex::comma) {
					lexical::lex_wrapper& top = st.top();
					while (top.lex_type != lex::lb) {
						out.push_back(top);
						st.pop();
						top = st.top();
					}
				}
				else if (lw.lex_type == lex::lb) {
					st.push(lw);
				}
				else if (lw.lex_type == lex::rb) {
					lexical::lex_wrapper& top = st.top();
					while (top.lex_type != lex::lb) {
						out.push_back(top);
						st.pop();
						top = st.top();
					}
					st.pop();

					if (!st.empty()) {
						top = st.top();
						if (top.lex_type == lex::function) {
							out.push_back(top);
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
