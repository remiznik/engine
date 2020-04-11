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
				void interpret(const vector<ExprPtr>& expr);

				core::Value visit(Literal* expr);
				core::Value visit(Grouping* expr);
				core::Value visit(Unary* expr);
				core::Value visit(Binary* expr);
				core::Value visit(Stmt* expr);
				core::Value visit(StmtPrint* expr);				

				core::Value evaluate(Expr* expr);
			private:
				void execute(Expr* expr);
		};
	}
}