#include "LRParser.hpp"

namespace miniparser {

	// we peel off the parantheses
	// lambda expression only has four situations
	// expr -> constant
			// | variable 
			// | expr expr
			// | lambda variable: expr
	// only constant and variable which are two terminal symbols can contain real pyylex text.

	enum Kind {
		CONST_NODE,
		VAR_NODE,
		APP_NODE,
		ABS_NODE,
		ROOT_NODE
	};

	class Node
	{
	public:
		int kind;
		std::string data;
		// for abs node children 0 = variable 1 = expr
		// for app node children 0 = expr 1 = expr
		// for variable no children data = pyytext
		// for constant it's complicated data = pyytext and no children here
		// which means we treat lambda expr as a constant if constant is Y, AND, OR ...
		std::vector<Node> binders;
		std::vector<Node> children;
		Node() = default;
		~Node() = default;
		void print() const {
			switch(kind)
			{
				case ROOT_NODE: {
					for (auto const& x: children)
					{
						x.print();
						std::cout<<";"<<std::endl;
					}
				}
				break;
				case ABS_NODE: {
					this->lambdaRecursivePrint(binders, children);
				}
				break;
				case APP_NODE: {
					std::cout<<"(";
					children[0].print();
					std::cout<<" ";
					children[1].print();
					std::cout<<")";
				}
				break;
				case VAR_NODE: {
					std::cout<<data;
				}
				break;
				case CONST_NODE: {
					std::cout<<data;
				}
				break;
			}
		}
		void lambdaRecursivePrint(std::vector<Node> binders, std::vector<Node> children) const;
	};

	void Node::lambdaRecursivePrint(std::vector<Node> binders, std::vector<Node> children) const {
		int i = 0;
		for(; i < binders.size(); i++) {
			std::cout<<"(lambda ";
			binders[i].print();
			std::cout<<":";
		}
		std::cout<<" ";
		children[0].print();
		for(i = 0; i < binders.size(); i++) {
			std::cout<<")";
		}
	}

	// CONSTANT ::= ...
	void function0(std::deque<boost::any>& valueStack) {
		Node element;
		element.kind = CONST_NODE;
		element.data = minilex::pyytext[0];
		attributeForParentNode = element;
	}

	// VARIABLE_EXPR ::= LP VARIABLE RP
	//				| VARIABLE
	void function1(std::deque<boost::any>& valueStack) {
		Node element;
		element.kind = VAR_NODE;
		element.data = minilex::pyytext[1];
		attributeForParentNode = element;
	}

	void function2(std::deque<boost::any>& valueStack) {
		Node element;
		element.kind = VAR_NODE;
		element.data = minilex::pyytext[0];
		attributeForParentNode = element;
	}

	// LAM_EXPR ::= LP LAM_EXPR RP ----3
	// 		| CONSTANT
	// 		| VARIABLE_EXPR ----4
	// 		| LAM_EXPR LAM_EXPR ----5
	//		| LAMBDA_ABS ----6

	// LAMBDA_ABS ::= BINDERS LAM_EXPR ----10
	// BINDERS	::= lambda VARIABLE_EXPR COLON BINDERS ----11
	// 		| lambda VARIABLE_EXPR COLON ----12
	void function10(std::deque<boost::any>& valueStack) {
		Node binders = boost::any_cast<Node>(valueStack.at(valueStack.size() - 2));
		Node lamexpr = boost::any_cast<Node>(valueStack.at(valueStack.size() - 1));
		Node element;
		element.kind = ABS_NODE;
		element.binders = binders.binders;
		element.children.push_back(lamexpr);
		attributeForParentNode = element;
	}

	void function11(std::deque<boost::any>& valueStack) {
		Node binders = boost::any_cast<Node>(valueStack.at(valueStack.size() - 1));
		std::reverse(binders.binders.begin(),binders.binders.end());
		binders.binders.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 3)));
		std::reverse(binders.binders.begin(),binders.binders.end());
		binders.children.clear();
		attributeForParentNode = binders;
	}

	void function12(std::deque<boost::any>& valueStack) {
		Node binders;
		binders.binders.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 2)));
		binders.children.clear();
		attributeForParentNode = binders;
	}

	void function3(std::deque<boost::any>& valueStack) {
		attributeForParentNode = boost::any_cast<Node>(valueStack.at(valueStack.size() - 2));
	}

	void function4(std::deque<boost::any>& valueStack) {
		attributeForParentNode = boost::any_cast<Node>(valueStack.at(valueStack.size() - 1));
	}

	void function5(std::deque<boost::any>& valueStack) {
		Node element;
		element.kind = APP_NODE;
		element.children.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 2)));
		element.children.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 1)));
		attributeForParentNode = element;
	}

	void function6(std::deque<boost::any>& valueStack) {
		attributeForParentNode = boost::any_cast<Node>(valueStack.at(valueStack.size() - 1));
	}

	// EXPR :: LAM_EXPR SEMI
	// 	| EXPR LAM_EXPR SEMI 
	void function7(std::deque<boost::any>& valueStack) {
		attributeForParentNode = boost::any_cast<Node>(valueStack.at(valueStack.size() - 2));
	}

	void function8(std::deque<boost::any>& valueStack) {
		Node element;
		element.kind = ROOT_NODE;
		element.children.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 3)));
		element.children.push_back(boost::any_cast<Node>(valueStack.at(valueStack.size() - 2)));
		attributeForParentNode = element;
	}

	// STMT ::= EXPR EOI
	void function9(std::deque<boost::any>& valueStack) {
		attributeForParentNode = boost::any_cast<Node>(valueStack.at(valueStack.size() - 2));
	}

	// STMT ::= EXPR EOI
	// EXPR :: LAM_EXPR SEMI
	// 	| LAM_EXPR SEMI EXPR
	// LAM_EXPR ::= LP LAM_EXPR RP
	// 		| CONSTANT
	// 		| VARIABLE
	// 		| LAM_EXPR LAM_EXPR
	// 		| lamda VARIABLE_EXPR COLON LAM_EXPR
	// VARIABLE_EXPR ::= VARIABLE
	//				| LP VARIABLE RP
	// CONSTANT ::= CONS
	// 		| HEAD
	// 		| TAIL
	// 		| Y
	// 		| IF
	// 		| NUM
	// 		| CHAR
	// 		| AND
	// 		| OR
	// 		| NOT
	// 		| PLUS 
	// 		| MINUS
	// 		| TIMES
	// 		| DIVIDE
	// 		| TRUE
	// 		| FALSE

	std::map<int, Symbols> symbolMap = {
		{STMT, miniparser::Symbols(STMT, false, {{EXPR, EOI}})},
		{EXPR, miniparser::Symbols(EXPR, false, {{EXPR, LAM_EXPR, SEMI}, {LAM_EXPR, SEMI}})},
		{LAM_EXPR, miniparser::Symbols(LAM_EXPR, false, {{LP, LAM_EXPR, RP}, 
			{CONSTANT}, {VARIABLE_EXPR}, {LAM_EXPR, LAM_EXPR}, {LAMBDA_ABS}})},
		{LAMBDA_ABS, miniparser::Symbols(LAMBDA_ABS, false, {{BINDERS, LAM_EXPR}})},
		{BINDERS, miniparser::Symbols(BINDERS, false, {{lambda, VARIABLE_EXPR, COLON, BINDERS}, 
			{lambda, VARIABLE_EXPR, COLON}})},
		{VARIABLE_EXPR, miniparser::Symbols(VARIABLE_EXPR, false, {{LP, VARIABLE, RP}, {VARIABLE}})},
		{CONSTANT, miniparser::Symbols(CONSTANT, false, {{CONS}, {HEAD}, {TAIL}, {Y}, {IF}, {NUM},
		 {CHAR}, {AND}, {OR}, {NOT}, {PLUS}, {MINUS}, {TIMES}, {DIVIDE}, {TRUE}, {FALSE}})},
		
		{EOI , miniparser::Symbols(EOI, false, {})},
		{SEMI , miniparser::Symbols(SEMI, false, {})},
		{PLUS , miniparser::Symbols(PLUS, false, {})},
		{MINUS , miniparser::Symbols(MINUS, false, {})},
		{TIMES , miniparser::Symbols(TIMES, false, {})},
		{DIVIDE , miniparser::Symbols(DIVIDE, false, {})},
		{LP , miniparser::Symbols(LP, false, {})},
		{RP , miniparser::Symbols(RP, false, {})},
		{TRUE , miniparser::Symbols(TRUE, false, {})},
		{FALSE , miniparser::Symbols(FALSE, false, {})},
		{COLON , miniparser::Symbols(COLON, false, {})},
		{VARIABLE , miniparser::Symbols(VARIABLE, false, {})},
		{NUM , miniparser::Symbols(NUM, false, {})},
		{CHAR , miniparser::Symbols(CHAR, false, {})},
		{AND , miniparser::Symbols(AND, false, {})},
		{OR , miniparser::Symbols(OR, false, {})},
		{NOT, miniparser::Symbols(NOT, false, {})},
		{lambda , miniparser::Symbols(lambda, false, {})},
		{IF , miniparser::Symbols(IF, false, {})},
		{HEAD , miniparser::Symbols(HEAD, false, {})},
		{TAIL , miniparser::Symbols(TAIL, false, {})},
		{CONS , miniparser::Symbols(CONS, false, {})},
		{Y , miniparser::Symbols(Y, false, {})},
		{UNKNOWN_SYMBOL , miniparser::Symbols(UNKNOWN_SYMBOL, false, {})},
	};

	std::map<std::pair<int, int>, std::function<void(std::deque<boost::any>&)>> functionMap = {
		{{CONSTANT, 0}, function0},
		{{CONSTANT, 1}, function0},
		{{CONSTANT, 2}, function0},
		{{CONSTANT, 3}, function0},
		{{CONSTANT, 4}, function0},
		{{CONSTANT, 5}, function0},
		{{CONSTANT, 6}, function0},
		{{CONSTANT, 7}, function0},
		{{CONSTANT, 8}, function0},
		{{CONSTANT, 9}, function0},
		{{CONSTANT, 10}, function0},
		{{CONSTANT, 11}, function0},
		{{CONSTANT, 12}, function0},
		{{CONSTANT, 13}, function0},
		{{CONSTANT, 14}, function0},
		{{CONSTANT, 15}, function0},
		{{VARIABLE_EXPR, 0}, function1},
		{{VARIABLE_EXPR, 1}, function2},
		{{LAM_EXPR, 0}, function3},
		{{LAM_EXPR, 1}, function4},
		{{LAM_EXPR, 2}, function4},
		{{LAM_EXPR, 3}, function5},
		{{LAM_EXPR, 4}, function6},
		{{EXPR, 0}, function8},
		{{EXPR, 1}, function7},
		{{STMT, 0}, function9},
		{{LAMBDA_ABS, 0}, {function10}},
		{{BINDERS, 0}, {function11}},
		{{BINDERS, 1}, {function12}}
	};
}

int main(int argc, char const *argv[])
{
	std::unique_ptr<miniparser::Lexer> minilexer(new miniparser::Lexer());
	minilexer->initMacro({
		{std::string("CHAR"), std::string("^['][[:alnum:]][']")},
		{std::string("AROP"), std::string("[+-|\\*|\\/]")},
		{std::string("MISCEL"), std::string("[:|;|\\(|\\)]")},
		{std::string("VARIABLE"), std::string("^([[:alpha:]])+(([[:alnum:]])*)")},
		{std::string("NUM"), std::string("^[-+]?[0-9]*\\.?[0-9]+")},
	});

	minilexer->addRegularExprs({
		std::string("{CHAR}"),
		std::string("{NUM}"),
		std::string("{VARIABLE}"),
		std::string("{MISCEL}"),
		std::string("{AROP}"),
		std::string(".")
	});

	std::unique_ptr<miniparser::LRGotoTableEmitter> minitable(new miniparser::LRGotoTableEmitter());
	minitable->crunchRules(miniparser::symbolMap);

	miniparser::LRParser parser(std::move(minilexer), std::move(minitable));
    parser.parse(miniparser::symbolMap, miniparser::functionMap);

    miniparser::Node root = boost::any_cast<miniparser::Node>(miniparser::attributeForParentNode);

    root.print();

	return 0;
}