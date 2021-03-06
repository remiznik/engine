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
	core::Value visit(ClassExpr* expr) override;
	core::Value visit(GetExpr* expr) override;
	core::Value visit(SetExpr* expr) override;
	core::Value visit(This* expr) override;
	core::Value visit(Super* expr) override;

	void resolve(const vector<ExprPtr>& statements);

private:
	enum class FunctionType
	{
		NONE,
		FUNCTION,
		INITIALIZER,
		METHOD
	};

	enum class ClassType
	{
		NONE,
		CLASS,
		SUPERCLASS
	};


private:	
	void resolveStmt(Expr* statement);
	void resolveLockal(Expr* expr, Token name);
	void resolveFunction(Function* expr, FunctionType type);
	void declare(Token name);
	void define(Token name);

	void beginScope();
	void endScope();

private:
	Interpreter* interpreter_;
	vector<map<string, bool>> scopes_;		
	core::Logger& logger_;
	FunctionType currentFunction_ {FunctionType::NONE};
	ClassType currentClass_{ ClassType::NONE };
};
}
}