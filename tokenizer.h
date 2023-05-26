#pragma once
#include <string>
#include <optional>
#include "maps.h"
#include <algorithm>

using namespace defs;

namespace solver
{
	class tokenizer {
		mutable std::string _equation;
		mutable size_t _len{};
		std::vector<std::string> _variables;
		std::map<std::string, std::string> _equations;

		void remove_whites() const;

		static bool is_word_char(const char c) { return std::isalnum(c) || c == '_'; }

		[[nodiscard]] bool braces_are_balanced() const;

		std::string read_word(size_t& idx) const;

		template<typename T>
		std::optional<T> read_number(size_t& idx) const;

		lex_wrapper classify(lex prev_lex, size_t& idx) const;

	public:
		explicit tokenizer(std::vector<std::string> var_names, std::map<std::string, std::string> equations) :
			_variables(std::move(var_names)), _equations(std::move(equations)) {
			for (const auto& [name, _] : _equations)
				_variables.insert(_variables.end(), name);
		}

		tokenizer() : tokenizer({}, {}) {}

		//tokenizer& operator=(const std::string& s) {}

		[[nodiscard]] std::vector<lex_wrapper> parseSingle(const std::string& equaiton) const;

		[[nodiscard]] std::variant<std::vector<std::string>, error> parse() const;
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
