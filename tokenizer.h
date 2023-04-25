#pragma once
#include <string>
#include <optional>
#include "maps.h"
#include <algorithm>

#include "fn_args_counter.h"

using namespace defs;

namespace solver
{
	class tokenizer {
		std::string _equation;
		size_t _len{};

		std::vector<std::string> _variables;

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
				return can_follow(prev_lex, lex::end) ? lex_end_w : lex_bad_tokens_seq_w;
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
					const std::vector<lex>& candidates = lex_oper_map.at(*it);
					const auto lexIt = std::ranges::find_if(candidates, [prev_lex](lex l) {
						// find 1st which has prev_lex allowed
						return can_follow(prev_lex, l);
						//return std::find(order.at(l).begin(), order.at(l).end(), prev_lex) != order.at(l).end();
				  });

				  if (lexIt == candidates.end())
					  return lex_bad_tokens_seq_w;

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
					return can_follow(prev_lex, lex::function) ? lex_wrapper{ lex::function, *itf } : lex_bad_tokens_seq_w;
				}

				// variable
				const auto itv = std::ranges::find_if(_variables, [&w](const std::string& s) {
					return std::ranges::equal(s, w, [](const char a, const char b) {
						return std::tolower(a) == std::tolower(b);
					});
				});
				if (itv != _variables.end()) {
					return can_follow(prev_lex, lex::variable) ? lex_wrapper{ lex::variable, w } : lex_bad_tokens_seq_w;
				}

				// const
				if (const auto itc = std::ranges::find(lex_consts::names, w); itc != lex_consts::names.end()) {
					return can_follow(prev_lex, lex::constant) ? lex_wrapper{ lex::constant, *itc } : lex_bad_tokens_seq_w;
				}
			}

			return lex_wrapper{ lex::error, error::unknown_token };
		}

	public:
		explicit tokenizer(std::string equation, std::vector<std::string> var_names) :
	    _equation(std::move(equation)), _variables(std::move(var_names)) {
			  remove_whites();
		}

		tokenizer& operator=(const std::string& s) {
			_equation = s;
			remove_whites();
			
			return *this;
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

      if (lw.lex_type != lex::error) {
				// remove unary_pluses and begin
				const auto itr = std::ranges::remove_if(lexes, [](const auto& l) { return l.lex_type == lex::unary_plus; }).begin();
				lexes.erase(itr, lexes.end());
				lexes.erase(lexes.begin());

				// compute and validate # of arguments to functions
        const std::map<size_t, size_t> fn_args_count = fn_args_counter::get_fn_arguments_count(lexes);
        if (const error err = fn_args_counter::validate_arguments(lexes, fn_args_count); err == error::success) {
					if (!lexes.empty()) {
						lw = classify(lexes.back().lex_type, idx);
						lexes.push_back(lw);
					}
					else
						lexes.emplace_back(lex::error, error::empty_input);
				}
				else
					lexes.emplace_back(lex::error, err);

				for (size_t i = 0, n = lexes.size(); i < n; ++i) {
					if (lexes[i].lex_type == lex::function) {
						if (fn_args_count.contains(i))
							lexes[i].n_args = fn_args_count.at(i);
					}
					else
						lexes[i].n_args = args_count(lexes[i].lex_type);
				}
			}

			return lexes;
		}
	};
}
