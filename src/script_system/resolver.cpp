#include "resolver.h"

#include "interpreter.h"

namespace script_system {
namespace parser {
	namespace 
	{
		class ResolverMessage : public core::LogMessage
		{
		public:
			ResolverMessage(const string& name)
				: name_(name)
			{}

			string toString() override
			{
				string message("Cannot read local variable ");
				message.append(name_);
				message.append(" in its own initialize.");
				return message;
			}

		private:
			string name_;
		};

		class VariableAllradyMessage : public core::LogMessage
		{
		public:
			VariableAllradyMessage(const string& name)
				: name_(name)
			{}

			string toString() override
			{
				string message("Variable with this ");
				message.append(name_);
				message.append(" already declared in this scope.");
				return message;
			}

		private:
			string name_;
		};
	}

	Resolver::Resolver(Interpreter* interpreter, core::Logger& logger)
		: interpreter_(interpreter), logger_(logger)
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

	void Resolver::beginScope()
	{
		scopes_.push_back(map<string, bool>());
	}

	void Resolver::endScope()
	{
		scopes_.pop_back();
	}

	core::Value Resolver::visit(Var* expr)
	{
		declare(expr->name);
		if (expr->initilize)
		{
			resolveStmt(expr->initilize.get());
		}
		define(expr->name);
		return core::Value();
	}

	core::Value Resolver::visit(Variable* expr)
	{
		if (!scopes_.empty())
		{
			auto it = scopes_.back().find(expr->name.lexeme);
			if ( it != scopes_.back().end())
			{				
				if (it->second == false)
				{
					auto msg = ResolverMessage(expr->name.lexeme);
					logger_.write(msg);
				}
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
		declare(expr->name);                              
    	define(expr->name);

		ScopeGuard<FunctionType> guard(currentFunction);
		beginScope();
		for (auto param: expr->params)
		{
			declare(param);
			define(param);
		}
		resolve(expr->body);
		endScope();
		return core::Value();
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
		if (currentFunction == FunctionType::NONE)
		{
			logger_.write("Cannot return from top-level code.");
		}

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
		for (int i = scopes_.size() - 1; i >= 0; --i )
		{
			if (scopes_[i].find(name.lexeme) != scopes_[i].end())
			{
				interpreter_->resolve(expr, scopes_.size() - 1 + i);
				return;
			}
		}
	}
	void Resolver::declare(Token name)
	{
		
		if (scopes_.empty()) return;
		
		auto it = scopes_.back().find(name.lexeme);
		if (it == scopes_.back().end())
		{
			scopes_.back().emplace(name.lexeme,false);
		}
		else
		{
			auto msg = VariableAllradyMessage(name.lexeme);
			logger_.write(msg);
		}
		
	}

	void Resolver::define(Token name)
	{
		if (scopes_.empty()) return;

		scopes_.back()[name.lexeme] = true;
	}

}
}