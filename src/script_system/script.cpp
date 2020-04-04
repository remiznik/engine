#include "script.h"

#include "core/value.h"
#include "ast.h"
#include "ast_printer.h"

namespace script_system
{
	using namespace parser;

	Script::Script()
		: scanner_(logger_)
	{	
	}

	void Script::run()
	{
		shared_ptr<Expr> exp = std::make_shared<Binary>(
				std::make_shared<Unary>(Token(TokenType::MINUS, "-", core::Value(), -1), std::make_shared<Literal>(core::Value(123))),
				Token(TokenType::STAR, "*", core::Value(), -1),
				std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)))
			);
		//shared_ptr<Expr> exp = std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)));
			
		AstPrinter printer;
		auto result = printer.print(exp);
		double x = 3;
	}
}
