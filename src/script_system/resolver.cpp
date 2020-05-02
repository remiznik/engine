#include "resolver.h"

namespace script_system {
namespace parser {
	namespace 
	{
		class ResolverMessage : public core::LogMessage
		{
		public:
			ResolverMessage(const string& message)
				: m_message(message)
			{}

			string toString() override
			{
				return m_message;
			}

		private:
			string m_message;
		};
	}

	Resolver::Resolver(Interpreter* interpreter, core::Logger& logger)
		: m_interpreter(interpreter), logger_(logger)
	{	
	}

	core::Value Resolver::visit(Block* expr)
	{
		beginScope();
		resolve(expr->statements);
		endScope();
		return core::Value();
	}

	void Resolver::resolve(const vector<ExprPtr>& statements)
	{
		for (auto statement : statements)
		{
			resolveStmt(statement.get());
		}
	}

	void Resolver::resolveStmt(Expr* statement)
	{
		statement->accept(this);
	}

	void Resolver::resolveExpr(Expr* expr)
	{
		expr->accept(this);
	}

	void Resolver::beginScope()
	{
		m_scopes.push_back(map<string, bool>());
	}

	void Resolver::endScope()
	{
		m_scopes.pop_back();
	}

	core::Value Resolver::visit(Var* expr)
	{
		declare(expr->name);
		if (expr->initilize)
		{
			resolveExpr(expr->initilize.get());
		}
		define(expr->name);
	}

	core::Value Resolver::visit(Variable* expr)
	{
		if (!m_scopes.empty())
		{
			if (m_scopes.back()[expr->name.lexeme] == false)
			{				
				auto msg = ResolverMessage(
					"Cannot read local variable  in its own initialize.");
				logger_.write(msg);
			}
		}

		resolveLockal(expr, expr->name);
		return core::Value();
	}

	core::Value Resolver::visit(Assign* expr)
	{
		resolveStmt(expr->value.get());
		resolveLockal(expr, expr->name);
		return core::Value();
	}

	core::Value Resolver::visit(Function* expr)
	{
		beginScope();
		for (auto param: expr->params)
		{
			declare(param);
			define(param);
		}
		resolve(expr->body);
		endScope();
	}

	core::Value Resolver::visit(Stmt* expr) 
	{
		resolveStmt(expr->expr.get());
		return core::Value();
	}

	core::Value Resolver::visit(IfExpr* expr) 
	{
		resolveStmt(expr->condition.get());
		resolveStmt(expr->thenBranch.get());
		if (expr->elseBranch) resolveStmt(expr->elseBranch.get());
		return core::Value();
	}

	core::Value Resolver::visit(StmtPrint* expr)
	{
		resolveStmt(expr->print.get());
		return core::Value();
	}

	core::Value Resolver::visit(Return* expr) 
	{
		if (expr->value != nullptr)
		{
			resolveStmt(expr->value.get());
		}
		return core::Value();
	}
	core::Value Resolver::visit(WhileExpr* expr)
	{
		resolveStmt(expr->condition.get());
		resolveStmt(expr->body.get());
		return core::Value();
	}

	core::Value Resolver::visit(Binary* expr)
	{
		resolveStmt(expr->left.get());
		resolveStmt(expr->right.get());
		return core::Value();
	}

	core::Value Resolver::visit(Call* expr)
	{
		resolveStmt(expr->callee.get());
		for (auto arg : expr->arguments)
		{
			resolveStmt(arg.get());
		}
		return core::Value();
	}

	core::Value Resolver::visit(Grouping* expr) 
	{
		resolveStmt(expr->expression.get());
		return core::Value();
	} 

	core::Value Resolver::visit(Literal* expr)
	{
		return core::Value();
	}

	core::Value Resolver::visit(Logical* expr)
	{
		resolveStmt(expr->left.get());
		resolveStmt(expr->right.get());
		return core::Value();
	}

	core::Value Resolver::visit(Unary* expr)
	{
		resolveStmt(expr->right.get());
		return core::Value();
	}

	void Resolver::resolveLockal(Expr* expr, Token name)
	{
		for (int i = m_scopes.size() - 1; i >= 0; --i )
		{
			if (m_scopes[i].find(name.lexeme) != m_scopes[i].end())
			{
				//m_interpreter->resolve(expr, m_scopes.size() - 1 + i);
				return;
			}
		}
	}
	void Resolver::declare(Token name)
	{
		if (m_scopes.empty()) return;

		m_scopes.back()[name.lexeme] = false;
	}

	void Resolver::define(Token name)
	{
		if (m_scopes.empty()) return;

		m_scopes.back()[name.lexeme] = true;
	}

}
}