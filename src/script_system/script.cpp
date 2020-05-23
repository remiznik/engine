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

	

	Script::Script()
		: scanner_(logger_), interpreter_(logger_)
	{	
	}

	void Script::run(const string& fileName)
	{
		auto s = reader_.read(fileName);
		execute(s);			
	}

	void Script::execute(const string& script)
	{
		auto tokens = scanner_.scan(script);		
		Parser parser(logger_, tokens);
		auto t = parser.parse();		
		Resolver res(&interpreter_, logger_);
		res.resolve(t);
		interpreter_.interpret(t);
	}

	void Script::registreFunction(const string& name, const shared_ptr<class core::Callable>& fnc)
	{
		interpreter_.registreFunction(name, fnc);
	}
}
