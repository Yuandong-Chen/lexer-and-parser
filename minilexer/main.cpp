#include "MiniLex.hpp" // you are required to modify yylex() for your own purposes in MiniLex.cpp 

int main(int argc, char const *argv[])
{
	std::unique_ptr<minilex::MiniLex> mini(new minilex::MiniLex());
	mini->initMacro({
		{std::string("CHAR"), std::string("^['][[:alnum:]][']")},
		{std::string("AROP"), std::string("[+-|\\*|\\/]")},
		{std::string("MISCEL"), std::string("[:|;|\\(|\\)]")},
		{std::string("VARIABLE"), std::string("^([[:alpha:]])+(([[:alnum:]])*)")},
		{std::string("NUM"), std::string("^[-+]?[0-9]*\\.?[0-9]+")},
	});

	mini->addRegularExprs({
		std::string("{CHAR}"), // char
		std::string("{NUM}"),
		std::string("{VARIABLE}"),
		std::string("{MISCEL}"),
		std::string("{AROP}"),
		std::string(".")
	});

	while(mini->yylex());
	return 0;
}