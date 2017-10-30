all:
	g++ -std=c++11 desuger_enriched_lambda_calculus.cpp minilexer/MiniLex.cpp minilexer/RegularExp.cpp minilexer/MacroHandler.cpp 