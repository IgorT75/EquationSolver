#pragma once
#include <stack>
#include <variant>

#include "maps.h"

namespace solver
{
	class rpn_solver
	{
		std::vector<lexical::Term> _terms;
		size_t _len;
		public:
		  rpn_solver(std::vector<lexical::Term> terms) : _terms(std::move(terms)), _len(terms.size()) {}

		[[nodiscard]] std::variant<double, bool> solve() const {
			  auto it = _terms.begin();
			  std::stack<std::variant<double, bool>> st;
			  while (it != _terms.end() || st.size() != 1)
			  {
				  if (it->index() == 0) {
					  st.push(std::get<double>(*it));
				  }
				  else if (it->index() == 1) {
					  st.push(std::get<bool>(*it));
				  }
				  else {
					  std::vector<std::variant<double, bool>> dVec;
					  size_t i = 0;
					  const size_t nArgs = lexical::term_args_map[it->index()];
					  while (!st.empty() && i++ < nArgs) {
						  dVec.push_back(st.top());
						  st.pop();
					  }

					  if (dVec.size() != nArgs)
						  return std::numeric_limits<double>::quiet_NaN();

					  std::variant<double, bool> ret;
					  switch (it->index()) {
						  case 2: { // double from double
							  auto op2 = std::get<2>(*it);
							  auto f2 = op2.executor();
							  ret = f2(std::get<double>(dVec[0]));
							  break;
						  }
						  case 3: { // double from two doubles
							  auto op3 = std::get<3>(*it);
							  auto f3 = op3.executor();
							  ret = f3(std::get<double>(dVec[1]), std::get<double>(dVec[0]));
							  break;
						  }
						  case 4: { // If function: returns double from bool and two doubles
							  auto op4 = std::get<4>(*it);
							  auto f4 = op4.executor();
							  ret = f4(std::get<bool>(dVec[2]), std::get<double>(dVec[1]), std::get<double>(dVec[0]));
							  break;
						  }
						  case 5: { // returns boolean from two doubles
							  auto op5 = std::get<5>(*it);
							  auto f5 = op5.executor();
							  ret = f5(std::get<double>(dVec[1]), std::get<double>(dVec[0]));
							  break;
						  }
						  case 6: { // returns boolean from two booleans
							  auto op6 = std::get<6>(*it);
							  auto f6 = op6.executor();
							  if (dVec[0].index() != 1 || dVec[1].index() != 1)
								  return std::numeric_limits<double>::quiet_NaN();
							  ret = f6(std::get<bool>(dVec[1]), std::get<bool>(dVec[0]));
							  break;
						  }
						  case 7: { // returns const double
							  auto op7 = std::get<7>(*it);
							  auto f7 = op7.executor();
							  ret = f7();
							  break;
						  }
					  }

					  st.push(ret);
				  }
				  ++it;
			  }

			  if (st.size() == 1)
				  return st.top();
			  return std::numeric_limits<double>::quiet_NaN();
		  }
	};
}
