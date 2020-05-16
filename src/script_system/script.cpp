#include "script.h"

#include "core/value.h"
#include "core/callabel.h"

#include "ast.h"
#include "ast_printer.h"
#include "parser.h"
#include "resolver.h"

#include <iostream>

namespace script_system
{
	using namespace parser;

	class Output : public core::Callable
	{
	public:

		core::Value call(const vector<core::Value>& args) override
		{
			std::cout << args[0].to<string>() << std::endl;
			return core::Value();
		}

		string toString() const  override
		{
			return "Output";
		}
	};

	Script::Script()
		: scanner_(logger_), interpreter_(logger_)
	{	
		interpreter_.registreFunction("output", makeShared<Output>());
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
		
		Resolver res(&interpreter_, logger_);
		res.resolve(t);
		interpreter_.interpret(t);
		
		double x = 3;
	}
}
