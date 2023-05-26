#pragma once
#include "errors.h"
#include "maps.h"

namespace fn_args_counter {
	static void parse_fn_arguments_count(const std::vector<defs::lex_wrapper>& v, std::map<size_t, size_t>& m, size_t& idx) {
		// idx should point to function!!
		const size_t fn_idx = idx;
		size_t n_args = 0;
		++idx; // it assume to point to left brace
		size_t brace_count = 1;
		do {
			if (++idx >= v.size()) return;
			if (v[idx].lex_type == defs::lex::function) {
				parse_fn_arguments_count(v, m, idx);
			}
			else if (v[idx].lex_type == defs::lex::lb) {
				++brace_count;
			}
			else if (v[idx].lex_type == defs::lex::rb) {
				--brace_count;
			}
			else if (v[idx].lex_type == defs::lex::comma) {
				++n_args;
			}

			if (idx >= v.size())
				break;
		} while (brace_count != 0);

		m[fn_idx] = ++n_args;
	}

	[[nodiscard]] static std::map<size_t, size_t> get_fn_arguments_count(const std::vector<defs::lex_wrapper>& v) {
		// combine map of function index in vector of lex_wrapper to number of arguments
		std::map<size_t, size_t> m;
		for (size_t idx = 0, n = v.size(); idx < n; ++idx) {
			if (v[idx].lex_type == defs::lex::function) {
				parse_fn_arguments_count(v, m, idx);
			}
		}
		return m;
	}

	[[nodiscard]] static defs::error validate_arguments(const std::vector<defs::lex_wrapper>& v, const std::map<size_t, size_t>& m) {
		// combine map of function index in vector of lex_wrapper to number of arguments
		if (m.empty()) return defs::error::success;

		// now validate
		for (size_t idx = 0, n = v.size(); idx < n; ++idx) {
			if (!m.contains(idx)) continue;
			if (v[idx].lex_type == defs::lex::function) {
				const auto& fn_name = std::get<std::string>(v[idx].data);
				if (!defs::is_multi_arg_funcs(fn_name)) {
					auto fn = defs::func_map.at(fn_name);
					if (m.at(idx) != defs::term_args_map.at(fn.index()))
						return defs::error::wrong_args_count;
				}
			}
		}
		return defs::error::success;
	}
}
