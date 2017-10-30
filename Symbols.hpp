#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <boost/any.hpp>
#include "./minilexer/MiniLex.hpp"

#define TERMINAL_N 256

namespace miniparser {
	enum Symbol_N {
		// define your symbols
		STMT = TERMINAL_N,
		UNKNOWN_SYMBOL = 8,
		EXPR = 257,
		CONSTANT = 258,
		LAM_EXPR = 259,
		VARIABLE_EXPR = 260,
		LAMBDA_ABS = 261,
		BINDERS = 262,

		EOI = 0,
		SEMI = 1,
		PLUS = 2,
		MINUS = 3,
		TIMES = 4,
		DIVIDE = 5,
		LP = 6,
		RP = 7,
		TRUE = 9,
		FALSE = 10,
		COLON = 11,
		VARIABLE = 12,
		NUM = 13,
		CHAR = 14,

		AND = 100,
		OR = 101,
		NOT = 102,
		lambda = 103,
		IF = 104,
		HEAD = 105,
		TAIL = 106,
		CONS = 107,
		Y = 108
	};

	class Symbols
	{
	public:
		int value;
		std::vector<std::vector<int> > productions;
		std::vector<int> firstSet;
		bool isNullable;
		int priority = -1;

		Symbols(int symVal, bool nullable, std::vector<std::vector<int> > pro, int p = 0) {
			value = symVal;
			productions = pro;
			isNullable = nullable;
			if(symVal < TERMINAL_N)
			{
				priority = p;
				firstSet.push_back(symVal);
			}
		};
		~Symbols() = default;

		bool contains(int e){
			return (std::find(firstSet.begin(), firstSet.end(), e) != firstSet.end());
		};

		bool containsAll(const std::vector<int> other) {
			if(firstSet.size() < other.size())
				return false;

			for (int i = 0; i < other.size(); ++i)
			{
				if(std::find(firstSet.begin(), firstSet.end(), other[i]) == firstSet.end())
					return false;
			}
			return true;
		}
		
	};

	boost::any attributeForParentNode;
}


