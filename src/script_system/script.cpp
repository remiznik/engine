#include "script.h"

#include "core/value.h"
#include "ast.h"
#include "ast_printer.h"
#include "parser.h"
#include "resolver.h"

#include <iostream>

namespace script_system
{
	using namespace parser;

	Script::Script(const core::FileSystem& fileSystem)
		: fileSystem_(fileSystem), scanner_(logger_)
	{	
	}

	bool Script::initialize()
	{
		auto s = fileSystem_.readFile("../res/scripts/test.scr");

		auto tokens = scanner_.scan(s);
		Parser parser(logger_, tokens);
		auto t = parser.parse();
		//shared_ptr<Expr> exp = std::make_shared<Binary>(
		//		std::make_shared<Unary>(Token(TokenType::MINUS, "-", core::Value(), -1), std::make_shared<Literal>(core::Value(123))),
		//		Token(TokenType::STAR, "*", core::Value(), -1),
		//		std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)))
		//	);
		//shared_ptr<Expr> exp = std::make_shared<Grouping>(std::make_shared<Literal>(core::Value(23)));
		
		Resolver res(&interpreter_, logger_);
		res.resolve(t);
		interpreter_.interpret(t);

		double x = 3;
		return true;
	}

	void Script::update(int st, int x, int y)
	{
		interpreter_.update(st, x, y);
	}

	void Script::registreFunction(const string& name, const shared_ptr<class core::Callable>& fnc)
	{
		interpreter_.registreFunction(name, fnc);
	}



}
