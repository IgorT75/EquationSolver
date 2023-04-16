#pragma once
#include <string>
#include <optional>
#include "maps.h"

namespace defs
{
	class lex_parser {
		std::string _equation;
		size_t _len{};

		void remove_whites() {
			const auto it = std::ranges::remove(_equation, ' ').begin();
			_equation.erase(it, _equation.end());

			_len = _equation.size();
		}
	public:
		explicit lex_parser(std::string equation) : _equation(std::move(equation)) {
			remove_whites();
		}

		lex_parser& operator=(const std::string& s) {
			_equation = s;
			remove_whites();
			
			return *this;
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

		static bool is_word_char(char c) { return std::isalnum(c) || c == '_'; }

		std::string read_word(size_t& idx) const {
			const size_t idxStart = idx;
			while (idx < _len && is_word_char(_equation[idx])) {
				++idx;
			}
			std::string w = _equation.substr(idxStart, idx - idxStart);
			return w;
		}

		lex_wrapper classify(lex prev_lex, size_t& idx) const {
			if (idx > _len)
				return lex_wrapper{ lex::error };

			// check if last lex is correct
			if (idx == _len) {
				const bool prev_lex_ok = std::ranges::find(lex_order[lex::end], prev_lex) != lex_order[lex::end].end();
				return lex_wrapper{ prev_lex_ok ? lex::end : lex::error };
			}

			// number
			if (std::isdigit(_equation[idx])) {
				auto d = read_double(idx);
				return d.has_value() ? lex_wrapper{ lex::number, d.value() } : lex_wrapper{ lex::error };
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
						return lex_wrapper{ lex::error };

					idx += it->size();
					return lex_wrapper{ *lexIt, *it };
				}
			}

			if (std::isalpha(_equation[idx]) || _equation[idx] == '_') {
				const std::string w = read_word(idx);

				// function
				const auto itf = std::ranges::find(lex_functions::names, w);
				if (itf != lex_functions::names.end()) {
					return lex_wrapper{ lex::function, *itf };
				}

				// const
				const auto itc = std::ranges::find(lex_consts::names, w);
				if (itc != lex_consts::names.end()) {
					return lex_wrapper{ lex::constant, *itc };
				}
			}

			return lex_wrapper { lex::error };
		}

		[[nodiscard]] bool braces_are_matched() const {
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

		[[nodiscard]] std::vector<lex_wrapper> parse() const {
			std::vector lexes { lex_wrapper { lex::begin } };

			if (!braces_are_matched()) {
				lexes.emplace_back(lex::error);
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
				lw = classify(lexes.back().lex_type, idx);
				lexes.push_back(lw);
			}

			return lexes;
		}
	};
}
