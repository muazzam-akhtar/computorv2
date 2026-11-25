#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include "Complex.hpp"
#include "Polynomial.hpp"

struct Solution
{
	enum Kind				{	NONE, ALL, ROOTS	};
	
	Kind					kind;
	std::vector<Complex>	roots;
	bool					real;
	Solution() : kind(NONE), real(true) {}
};

class Solver
{
	public:
		static Solution	solve(const Polynomial &);
};

#endif
