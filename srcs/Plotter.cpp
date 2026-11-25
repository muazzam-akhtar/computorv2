#include "../includes/Plotter.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

static const double	TOO_BIG = 1e6;

struct Sample
{
	bool	ok;
	double	y;
	Sample() : ok(false), y(0) {}
};

static Sample	sampleAt(const Evaluator &eval, const Function &f, double x)
{
	Sample	s;
	try
	{
		Value	v = eval.call(f, Value(Complex(Rational(x))));
		if (v.isMatrix() || !v.scalar().isReal())
			return s;
		double	y = v.scalar().re().value();
		if (std::isnan(y) || std::isinf(y) || std::fabs(y) > TOO_BIG)
			return s;
		s.ok = true;
		s.y = y;
	}
	catch (const std::exception &)
	{}
	return s;
}

static int	rowOf(double y, double ymin, double ymax)
{
	return static_cast<int>(std::floor((ymax - y) / (ymax - ymin) * (Plotter::HEIGHT - 1) + 0.5));
}

static std::string	padLeft(const std::string &s, size_t width)
{
	return std::string(width > s.size() ? width - s.size() : 0, ' ') + s;
}

std::string	Plotter::plot(const Evaluator &eval, const Function &f, double lo, double hi)
{
	std::vector<Sample>	pts(WIDTH);
	double				ymin = 0, ymax = 0;
	bool				any = false;

	for (int c = 0; c < WIDTH; c++)
	{
		pts[c] = sampleAt(eval, f, lo + (hi - lo) * c / (WIDTH - 1));
		if (!pts[c].ok)
			continue;
		ymin = any ? std::min(ymin, pts[c].y) : pts[c].y;
		ymax = any ? std::max(ymax, pts[c].y) : pts[c].y;
		any = true;
	}
	if (!any)
		throw std::runtime_error("nothing to plot: the function has no real value on this range");
	if (ymax - ymin < 1e-9)
	{
		ymin -= 1;
		ymax += 1;
	}

	std::vector<std::string>	grid(HEIGHT, std::string(WIDTH, ' '));
	int							zeroRow = (ymin <= 0 && ymax >= 0) ? rowOf(0, ymin, ymax) : -1;
	int							zeroCol = (lo <= 0 && hi >= 0) ? static_cast<int>(std::floor((0 - lo) / (hi - lo) * (WIDTH - 1) + 0.5)) : -1;

	for (int r = 0; r < HEIGHT && zeroCol >= 0; r++)
		grid[r][zeroCol] = '|';
	for (int c = 0; c < WIDTH && zeroRow >= 0; c++)
		grid[zeroRow][c] = (c == zeroCol ? '+' : '-');
	int	prev = -1;
	for (int c = 0; c < WIDTH; c++)
	{
		if (!pts[c].ok)
		{
			prev = -1;
			continue;
		}
		int	row = rowOf(pts[c].y, ymin, ymax);
		grid[row][c] = '*';
		if (prev >= 0 && std::abs(row - prev) > 1 && std::abs(row - prev) <= HEIGHT / 2)
			for (int r = std::min(row, prev) + 1; r < std::max(row, prev); r++)
				grid[r][c] = '*';
		prev = row;
	}
	std::string	top = Rational(ymax).toString(), bottom = Rational(ymin).toString(), zero = "0";
	size_t	margin = std::max(top.size(), bottom.size());
	std::string	out;
	for (int r = 0; r < HEIGHT; r++)
	{
		std::string	label = (r == 0 ? top : (r == HEIGHT - 1 ? bottom : (r == zeroRow ? zero : "")));
		out += padLeft(label, margin) + " " + grid[r] + "\n";
	}
	std::string	left = Rational(lo).toString(), right = Rational(hi).toString();
	size_t		gap = WIDTH > left.size() + right.size() ? WIDTH - left.size() - right.size() : 1;
	out += std::string(margin + 1, ' ') + left + std::string(gap, ' ') + right;
	return out;
}