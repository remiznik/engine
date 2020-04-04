#include "script.h"

#include "core/value.h"
#include "ast.h"
#include "ast_printer.h"
#include "parser.h"

namespace script_system
{
	using namespace parser;

	Script::Script()
		: scanner_(logger_)
	{	
	}

	void Script::run()
	{
		auto tokens = scanner_.scan("1 + (2 + 3)");
		Parser parser(logger_, tokens);
		auto t = parser.parse();
		shared_ptr<Expr> exp = std::make_shared<Binary>(
				std::make_shared<Unary>(Token(TokenType::MINUS, "-", core::Value(), -1), std::make_shared<Literal>(core::Value(123))),
				Token(TokenType::STAR, "*", core::Value(), -1),
				std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)))
			);
		//shared_ptr<Expr> exp = std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)));
			
		AstPrinter printer;
		auto result = printer.print(t);
		double x = 3;
	}
}
