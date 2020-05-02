#pragma once

#include "core/types.h"
#include "core/logger.h"

#include "ast_visitor.h"
#include "ast.h"

namespace script_system {
namespace parser {
	class Interpreter;	

	class Resolver : public AstVisitor
	{
	public:
		Resolver(Interpreter* interpreter, core::Logger& logger);

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

	private:
		void resolve(const vector<ExprPtr>& statements);
		void resolveStmt(Expr* statement);
		void resolveExpr(Expr* statement);
		void resolveLockal(Expr* expr, Token name);
		void declare(Token name);
		void define(Token name);

		void beginScope();
		void endScope();

	private:
		Interpreter* m_interpreter;
		vector<map<string, bool>> m_scopes;		
		core::Logger& logger_;
	};
}
}