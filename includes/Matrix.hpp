#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <string>
#include <vector>
#include "../includes/Complex.hpp"

class Matrix
{
	public:
		Matrix(size_t, size_t);
		explicit				Matrix(const std::vector<std::vector<Complex>> &rows);
		static Matrix			identity(size_t);
		size_t					rows() const;
		size_t					cols() const;
		bool					isSquare() const;
		const Complex			&at(size_t, size_t) const;
		Complex					&at(size_t, size_t);
		std::string				sizeString() const;

		Matrix					operator+(const Matrix &) const;
		Matrix					operator-(const Matrix &) const;
		Matrix					operator*(const Matrix &) const;
		Matrix					operator-() const;
		Matrix					operator*(const Complex &) const;
		Matrix					operator/(const Complex &) const;
		Matrix					multiply(const Matrix &) const;
		Matrix					pow(const Complex &) const;

		bool					operator==(const Matrix &) const;
		std::string				toString() const;
		std::string				toInlineString() const;

	private:
		size_t					_rows;
		size_t					_cols;
		std::vector<Complex>	_data;

		void					requireSameSize(const Matrix &, const char *) const;
};

#endif