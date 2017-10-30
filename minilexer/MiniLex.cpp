#include "MiniLex.hpp"

namespace minilex {
	std::string yytext = "";
	std::string pyytext[YYTEXTBUFF];
	std::string::size_type yyleng = 0;
	std::istream& yyin = std::cin;
	std::ostream& yyout = std::cout;
	std::map<std::string, int> keywords = {
		{"EOI", 0},
		{"SEMI", 1},
		{"PLUS", 2},
		{"MINUS", 3},
		{"TIMES", 4},
		{"DIVIDE", 5},
		{"LP", 6},
		{"RP", 7},
		{"TRUE", 9},
		{"FALSE", 10},
		{"COLON", 11},
		{"VARIABLE", 12},
		{"NUM", 13},
		{"CHAR", 14},

		{"AND", 100},
		{"OR", 101},
		{"NOT", 102},
		{"lambda", 103},
		{"IF", 104},
		{"HEAD", 105},
		{"TAIL", 106},
		{"CONS", 107},
		{"Y", 108}
	};

	int yyline = 0;
	
	void MiniLex::initMacro(std::initializer_list<std::pair<std::string, std::string> > il) {
		std::unique_ptr<minilex::MacroHandler> uq(new minilex::MacroHandler());
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			uq->addMacro(*it);
		}

		std::unique_ptr<RegularExp> uqq(new RegularExp(std::move(uq)));

		reup.reset(uqq.release());
	}

	void MiniLex::addRegularExprs(std::initializer_list<std::string > il) {
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			reup->addRegularExp(*it);
		}
	}

	void MiniLex::removeRegularExprs(std::initializer_list<std::string > il) {
		auto it = il.begin();

		for (; it != il.end(); ++it)
		{
			reup->removeRegularExp(*it);
		}
	}

	int MiniLex::yylex() {
		std::string eaten;
		std::string pattern;
		if(yyin.eof() && linebuffer.empty() && flushed == false)
		{
			//std::cerr<<"EOF at line "<<yyline<<std::endl;
			return 0;
		}

		flushed = false;
		if(linebuffer.empty())
		{
			yyline++;
			std::getline(yyin, linebuffer);
		}

		eaten = reup->eat(linebuffer);
		yyleng = eaten.size();
		//pyytext = yytext;
		yytext = eaten;
		if(!reup->isEaten() && !yyin.eof())
		{
			yyin.clear();
			flushed = true;
			//std::cerr<<"IGNORE STRING at line "<<yyline<<" "<<linebuffer<<std::endl;
			return 8;
		}

		pattern = reup->matchPattern();

        /* you are required to modify the following code for your own purposes */
        if(pattern == std::string("{MISCEL}")) {
			//std::cerr<<"RECOGNIZE MISCEL at line "<<yyline<<' '<<eaten<<std::endl;
			if(yytext == ":")
				return keywords["COLON"];
			else if(yytext == ";")
				return keywords["SEMI"];
			else if(yytext == "(")
				return keywords["LP"];
			else if(yytext == ")")
				return keywords["RP"];
		}
		else if(pattern == std::string("{AROP}")) {
			//std::cerr<<"RECOGNIZE AROP at line "<<yyline<<' '<<eaten<<std::endl;
			if(yytext == "*")
				return keywords["TIMES"];
			else if(yytext == "/")
				return keywords["DIVIDE"];
			else if(yytext == "+")
				return keywords["PLUS"];
			else if(yytext == "-")
				return keywords["MINUS"];
		}
		else if(pattern == std::string("{CHAR}")) {
			//std::cerr<<"RECOGNIZE CHAR at line "<<yyline<<' '<<eaten<<std::endl;
			return keywords["CHAR"];
		}
		else if(pattern == std::string("{VARIABLE}")) {
			try {
				//std::cerr<<"try keywords at line "<<yyline<<' '<<eaten<<std::endl;
				if(keywords.at(yytext) >= 100)
					return keywords.at(yytext);
				else
					return keywords["VARIABLE"];
			}
			catch(...)
			{
				//std::cerr<<"RECOGNIZE variable at line "<<yyline<<' '<<eaten<<std::endl;
				return keywords["VARIABLE"];
			}
		}
		else if(pattern == std::string("{NUM}")) {
			//std::cerr<<"RECOGNIZE NUM at line "<<yyline<<' '<<eaten<<std::endl;
			return keywords["NUM"];
		}
		else if(pattern == std::string(".")) {
			//std::cerr<<"UNRECOGNIZE at line"<<yyline<<' '<<eaten<<std::endl;
			return 8;
		}
		
		return 0;
	}
}
