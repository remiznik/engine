#include "script.h"

#include "core/value.h"
#include "ast.h"
#include "ast_printer.h"
#include "parser.h"

#include <iostream>

namespace script_system
{
	using namespace parser;

	Script::Script()
		: scanner_(logger_)
	{	
	}

	void Script::run()
	{
		auto s = reader_.read("../res/scripts/test.scr");
		
		auto tokens = scanner_.scan(s);		
		Parser parser(logger_, tokens);
		auto t = parser.parse();
		//shared_ptr<Expr> exp = std::make_shared<Binary>(
		//		std::make_shared<Unary>(Token(TokenType::MINUS, "-", core::Value(), -1), std::make_shared<Literal>(core::Value(123))),
		//		Token(TokenType::STAR, "*", core::Value(), -1),
		//		std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)))
		//	);
		//shared_ptr<Expr> exp = std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)));
		
		interpreter_.interpret(t);
		
		double x = 3;
	}
}
