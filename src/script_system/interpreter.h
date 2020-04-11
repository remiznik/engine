#pragma once

#include "core/value.h"

#include "ast_visitor.h"
#include "ast.h"
#include "environment.h"

namespace script_system
{
	namespace parser {
		class Expr;

		class Interpreter : public AstVisitor
		{		
			public:
				void interpret(const vector<ExprPtr>& expr);

				core::Value visit(Literal* expr) override;
				core::Value visit(Grouping* expr) override; 
				core::Value visit(Unary* expr) override;
				core::Value visit(Binary* expr) override;
				core::Value visit(Stmt* expr) override;
				core::Value visit(StmtPrint* expr) override;
				core::Value visit(Var* expr) override;
				core::Value visit(Variable* expr) override;
				core::Value visit(Assign* expr) override;


				core::Value evaluate(Expr* expr);
			private:
				void execute(Expr* expr);

			private:
				Environment environment_;
		};
	}
}