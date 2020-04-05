#pragma once

#include "core/value.h"

#include "ast_visitor.h"
#include "ast.h"

namespace script_system
{
	namespace parser {
		class Expr;

		class Interpreter : public AstVisitor
		{		
			public:
				string interpret(ExprPtr expr);

				core::Value visit(Literal* expr);
				core::Value visit(Grouping* expr);
				core::Value visit(Unary* expr);
				core::Value visit(Binary* expr);

				core::Value evaluate(Expr* expr);
		};
	}
}