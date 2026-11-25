#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include <string>
#include "Evaluator.hpp"
#include "Function.hpp"

class Plotter
{
	public:
		static const int	WIDTH = 61;
		static const int	HEIGHT = 21;

		static std::string	plot(const Evaluator &, const Function &, double, double);
};

#endif
