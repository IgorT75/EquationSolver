#pragma once
namespace defs {

	enum class error {
		success,
		empty_input,
		braces_not_matched,
		bad_tokens_sequence,
		unknown_token,
		wrong_args_count,
		bad_operator,
		bad_number,
		out_of_range,
		wrong_type,
		graph_cycle,
		unknown,
	};

	inline static std::map<error, std::string> error_str {
		{ error::success, "success" },
		{ error::empty_input, "input is empty" },
		{ error::braces_not_matched, "parenthesis are not matched" },
		{ error::bad_tokens_sequence, "bad tokens sequence" },
		{ error::unknown_token, "unknown token" },
		{ error::wrong_args_count, "wrong number of arguments passed to function" },
		{ error::bad_operator, "bad operator" },
		{ error::out_of_range, "index is out of range" },
		{ error::wrong_type, "type of variable is wrong" },
		{ error::graph_cycle, "equation graph contains cycle" },
		{ error::unknown, "unknown" },
	};
}