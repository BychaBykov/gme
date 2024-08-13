#pragma once
//#include "interface.h"
#include <string>
#include "exprtk.hpp"

template <typename T>
class Func2arg
{
public:
	Func2arg(char* FuncFromInput) {
		if (FuncFromInput != "") {
			fexist = true;
			const std::string expression_string = FuncFromInput;
			symbol_table.add_variable("x", x);
			symbol_table.add_variable("y", y);
			symbol_table.add_constants();


			expression.register_symbol_table(symbol_table);

			parser_t parser;
			parser.compile(expression_string, expression);
		}
	}
	typedef exprtk::symbol_table<T> symbol_table_t;
	typedef exprtk::expression<T>   expression_t;
	typedef exprtk::parser<T>       parser_t;
	expression_t expression;
	symbol_table_t symbol_table;
	T x;
	T y;
	bool fexist = false;
	T value(T varx, T vary) {
		x = T(varx);
		y = T(vary);
		const T z = expression.value();
		//cout << x << " " << y<< " " << z <<endl;
		return z;
	}
	~Func2arg() {};
};
