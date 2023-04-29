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
		std::vector<std::string> _variables;

		void remove_whites();

		static bool is_word_char(const char c) { return std::isalnum(c) || c == '_'; }

		[[nodiscard]] bool braces_are_balanced() const;

		std::string read_word(size_t& idx) const;

		template<typename T>
		std::optional<T> read_number(size_t& idx) const;

		lex_wrapper classify(lex prev_lex, size_t& idx) const;

	public:
		explicit tokenizer(std::string equation, std::vector<std::string> var_names) :
			_equation(std::move(equation)), _variables(std::move(var_names)) {
			_len = _equation.size();
			//remove_whites();
		}

		tokenizer& operator=(const std::string& s) {
			_equation = s;
			_len = _equation.size();
			//remove_whites();
			return *this;
		}

		[[nodiscard]] std::vector<lex_wrapper> parse() const;
	};

	template<>
	inline std::optional<num_t> tokenizer::read_number(size_t& idx) const {
		// upon call idx guaranteed to be in bound and point to digit
		num_t d { 0 };
		do {
			d = num_t{ 10 } * d + (_equation[idx++] - 0x30);
		} while (idx < _len && std::isdigit(_equation[idx]));
	
		if (idx >= _len) return d;
	
		if (_equation[idx] == '.') {
			++idx;
			// requires at least 1 digit after .
			if (idx >= _len || !std::isdigit(_equation[idx])) return std::nullopt;
	
			num_t f{ 0 }, mul{ 1 };
			do {
				mul /= num_t{ 10 };
				f = f + mul * (_equation[idx++] - 0x30);
			} while (idx < _len && std::isdigit(_equation[idx]));
	
			d += f;
		}
	
		return d;
	}

	template<>
	inline std::optional<int> tokenizer::read_number(size_t& idx) const {
		// upon call idx guaranteed to be in bound and point to digit
		int d = 0;
		do {
			d = 10 * d + (_equation[idx++] - 0x30);
		} while (idx < _len && std::isdigit(_equation[idx]));

		return d;
	}

}
