#pragma once
#include <string>
#include <optional>
#include "maps.h"
#include <algorithm>

using namespace defs;

namespace solver
{
	class tokenizer {
		std::string _equation;
		size_t _len{};

		void remove_whites() {
			const auto it = std::ranges::remove(_equation, ' ').begin();
			_equation.erase(it, _equation.end());

			_len = _equation.size();
		}

		static bool is_word_char(char c) { return std::isalnum(c) || c == '_'; }

		[[nodiscard]] bool braces_are_balanced() const {
			size_t i = 0;
			for (const char c : _equation) {
				if (c == '(')
					++i;
				else if (c == ')') {
					if (i == 0)
						return false;
					--i;
				}
			}
			return i == 0;
		}

		std::string read_word(size_t& idx) const {
			const size_t idxStart = idx;
			while (idx < _len && is_word_char(_equation[idx])) {
				++idx;
			}
			std::string w = _equation.substr(idxStart, idx - idxStart);
			return w;
		}

		std::optional<double> read_double(size_t& idx) const {
			// upon call idx guaranteed to be in bound and point to digit
			double d = 0.0;
			do {
				d = 10.0 * d + (_equation[idx++] - 0x30);
			} while (idx < _len && std::isdigit(_equation[idx]));

			if (idx >= _len) return d;

			if (_equation[idx] == '.') {
				++idx;
				// requires at least 1 digit after .
				if (idx >= _len || !std::isdigit(_equation[idx])) return std::nullopt;

				double f = 0.0, mul = 1.0;
				do {
					mul /= 10.0;
					f = f + mul * (_equation[idx++] - 0x30);
				} while (idx < _len && std::isdigit(_equation[idx]));

				d += f;
			}

			return d;
		}

		lex_wrapper classify(lex prev_lex, size_t& idx) const {
			if (idx > _len)
				return lex_wrapper{ lex::error, error::out_of_range };

			// check if last lex is correct
			if (idx == _len) {
				const bool prev_lex_ok = std::ranges::find(lex_order[lex::end], prev_lex) != lex_order[lex::end].end();
				if (prev_lex_ok) return lex_end_w;
				return lex_wrapper{ lex::error, error::bad_tokens_sequence };
			}

			// number
			if (std::isdigit(_equation[idx])) {
				auto d = read_double(idx);
				return d.has_value() ? lex_wrapper{ lex::number, d.value() } : lex_wrapper{ lex::error, error::bad_number };
			}

			// operator
			if (!std::isalnum(_equation[idx]) && _equation[idx] != '_') {
				const auto it = std::ranges::find_if(lex_operators::names, [this, idx](const std::string& name) {
					if (idx + name.size() <= _len)
						return name == _equation.substr(idx, name.size());
					return false;
					});

				if (it != lex_operators::names.end()) {
					const std::vector<lex>& candidates = lex_oper_map[*it];
					auto& order = lex_order;
					const auto lexIt = std::ranges::find_if(candidates, [&order, prev_lex](lex l) {
						// find 1st which has prev_lex allowed
						return std::find(order[l].begin(), order[l].end(), prev_lex) != order[l].end();
						});

					if (lexIt == candidates.end())
						return lex_wrapper{ lex::error, error::bad_tokens_sequence };

					idx += it->size();
					return lex_wrapper{ *lexIt, *it };
				}
			}

			// function or const
			if (std::isalpha(_equation[idx]) || _equation[idx] == '_') {
				const std::string w = read_word(idx);

				// function
				const auto itf = std::ranges::find_if(lex_functions::names, [&w](const std::string& s) {
					return std::ranges::equal(s, w, [](const char a, const char b) {
						return std::tolower(a) == std::tolower(b);
					});
				});
				if (itf != lex_functions::names.end()) {
					const auto& order = lex_order[lex::function];
					if (std::ranges::find(order, prev_lex) != end(order))
						return lex_wrapper { lex::function, *itf };
					return lex_wrapper { lex::error, error::bad_tokens_sequence };
				}

				// const
				if (const auto itc = std::ranges::find(lex_consts::names, w); itc != lex_consts::names.end()) {
					return lex_wrapper{ lex::constant, *itc };
				}
			}

			return lex_wrapper{ lex::error, error::unknown_token };
		}

	public:
		explicit tokenizer(std::string equation) : _equation(std::move(equation)) {
			remove_whites();
		}

		tokenizer& operator=(const std::string& s) {
			_equation = s;
			remove_whites();
			
			return *this;
		}

		static void parse_fn_arguments_count(const std::vector<lex_wrapper>& v, std::map<size_t, size_t>& m, size_t& idx) {
			// idx should point to function!!
			const size_t fn_idx = idx;
			size_t n_args = 0;
			++idx; // it assume to point to left brace
			size_t brace_count = 1;
			do {
				++idx;
				if (v[idx].lex_type == lex::function) {
					parse_fn_arguments_count(v, m, idx);
				}
				else if (v[idx].lex_type == lex::lb) {
					++brace_count;
				}
				else if (v[idx].lex_type == lex::rb) {
					--brace_count;
				}
				else if (v[idx].lex_type == lex::comma) {
					++n_args;
				}

				if (idx >= v.size())
					break;
			} while (brace_count != 0);

			m[fn_idx] = ++n_args;
		}

		[[nodiscard]] static error validate_arguments(const std::vector<lex_wrapper>& v) {

			// combine map of function index to number of arguments
			std::map<size_t, size_t> m;
			for (size_t idx = 0, n = v.size(); idx < n; ++idx) {
				if (v[idx].lex_type == lex::function) {
					parse_fn_arguments_count(v, m, idx);
				}
			}

			if (m.empty())
				return error::success;

			// now validate
			for (size_t idx = 0, n = v.size(); idx < n; ++idx) {
				if (v[idx].lex_type == lex::function) {
					if (m.contains(idx)) {
						auto fn = defs::func_map[std::get<std::string>(v[idx].data)];
						if (m[idx] != defs::term_args_map[fn.index()])
							return error::wrong_args_count;
					}
				}
			}
			return error::success;
		}

		[[nodiscard]] std::vector<lex_wrapper> parse() const {
			std::vector lexes { lex_wrapper { lex::begin } };

			if (!braces_are_balanced()) {
				lexes.emplace_back(lex::error, error::braces_not_matched);
				return lexes;
			}

			size_t idx = 0;
			lex_wrapper lw { lex::begin };
			while (idx < _len) {
				lw = classify(lexes.back().lex_type, idx);
				lexes.push_back(lw);
				if (lw.lex_type == lex::error)
					break;
			}

			// add lex::end if no error
			if (lw.lex_type != lex::error) {

				// remove unary_pluses and begin
				const auto itr = std::ranges::remove_if(lexes, [](const auto& lw) { return lw.lex_type == lex::unary_plus; }).begin();
				lexes.erase(itr, lexes.end());
				lexes.erase(lexes.begin());

				if (validate_arguments(lexes) == error::success) {
					if (!lexes.empty()) {
						lw = classify(lexes.back().lex_type, idx);
						lexes.push_back(lw);
					}
					else
						lexes.emplace_back(lex::error, error::empty_input);
				}
				else
					lexes.emplace_back(lex::error, error::wrong_args_count);
			}

			return lexes;
		}
	};
}
