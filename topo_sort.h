#pragma once
#include <vector>
#include <unordered_map>
#include <variant>

#include "errors.h"

namespace solver {
	class topo_sort {
		enum class status { init, temp, done };

		std::vector<std::vector<size_t>> _graph;
		std::vector<size_t> _order;

		std::unordered_map<size_t, status> vert_status;
	public:
		topo_sort(std::vector<std::vector<size_t>> graph) : _graph(std::move(graph)) {}

		std::variant<std::vector<size_t>, defs::error> sort() {
			// fill vert_status
			const size_t n_vert = _graph.size();
			for (size_t i = 0; i < n_vert; ++i)
				vert_status[i] = status::init;

			// run algo
			for (size_t i = 0; i < n_vert; ++i) {
				if (vert_status[i] == status::done) continue;
				if (visit(i)) {
					return defs::error::graph_cycle;
				}
			}

			return _order;
		}

		// true means there is a cycle
		bool visit(const size_t n) {
			if (vert_status[n] == status::done) return false;
			if (vert_status[n] == status::temp) return true;

			vert_status[n] = status::temp;
			for (size_t i = 0, sz = _graph[n].size(); i < sz; ++i) {
				if (visit(_graph[n][i]))
					return true;
			}

			vert_status[n] = status::done;
			_order.push_back(n);
			return false;
		}
	};

}