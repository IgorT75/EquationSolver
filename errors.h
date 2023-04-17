#pragma once
namespace defs {

	enum class error {
		unknown,
		braces_not_matched,
		bad_tokens_sequence,
		unknown_token,
		wrong_num_args,
		bad_operator,
		bad_number,
		out_of_range,
		wrong_type,
	};

	inline static std::map<error, std::string> error_str {
		{ error::unknown, "unknown" },
		{ error::braces_not_matched, "parenthesis are not matched" },
		{ error::bad_tokens_sequence, "bad tokens sequence" },
		{ error::unknown_token, "unknown token" },
		{ error::wrong_num_args, "wrong number of arguments to function" },
		{ error::bad_operator, "bad operator" },
		{ error::out_of_range, "index is out of range" },
		{ error::wrong_type, "type of variable is wrong" },
	};
}