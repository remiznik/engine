#pragma once

#include "core/value.h"

#include "ast_visitor.h"
#include "ast.h"
#include "environment.h"

namespace script_system {
namespace parser {
class Expr;

class Interpreter : public AstVisitor
{		
	public:
		Interpreter();
		
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
		core::Value visit(Block* expr) override;
		core::Value visit(IfExpr* expt) override;
		core::Value visit(Logical* expt) override;
		core::Value visit(WhileExpr* expt) override;
		core::Value visit(Call* expr) override;
		core::Value visit(Function* expr) override;
		core::Value visit(Return* expr) override;
		core::Value visit(ClassExpr* expr) override;


		core::Value evaluate(Expr* expr);

		const shared_ptr<Environment>& environment() const 
		{
			return environment_;
		}

		void execute(const vector<ExprPtr>& statements, const shared_ptr<Environment>& env);
		void resolve(Expr* expr, int depth);
	private:
		void execute(Expr* expr);
		core::Value lookUpVariable(Token name, Expr* expr);
		

	private:
		map<Expr*, int> locals_;
		shared_ptr<Environment> environment_;
		shared_ptr<Environment> globals_;
};
}
}