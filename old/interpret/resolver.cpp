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
					logger_.write(ResolverMessage(expr->name.lexeme));
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
		resolveFunction(expr, FunctionType::FUNCTION);
		
		return core::Value();
	}

	void Resolver::resolveFunction(Function* expr, FunctionType type)
	{
		ScopeGuard<FunctionType> guard(currentFunction_);
		currentFunction_ = type;
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
		if (currentFunction_ == FunctionType::NONE)
		{
			logger_.write(core::LogMessage("Cannot return from top-level code."));
		}

		if (expr->value != nullptr)
		{
			if (currentFunction_ == FunctionType::INITIALIZER)
			{
				logger_.write(core::LogMessage("Cannot return a value form initializer"));
			}
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

	core::Value Resolver::visit(ClassExpr* expr)
	{
		ScopeGuard<ClassType> guard(currentClass_);
		currentClass_ = ClassType::CLASS;

		declare(expr->name);
		define(expr->name);

		if (expr->supperClass != nullptr)
		{
			if (expr->supperClass->name.lexeme == expr->name.lexeme)
			{
				logger_.write(core::LogMessage("A class cannot inherit itself"));
			}
			resolveStmt(expr->supperClass.get());
		}

		if (expr->supperClass != nullptr)
		{
			currentClass_ = ClassType::SUPERCLASS;
			beginScope();
			scopes_.back().emplace("super", true);
		}
		
		beginScope();
		scopes_.back().emplace("this", true);
		
		for (auto method : expr->methods)
		{
			FunctionType decl = FunctionType::METHOD;
			resolveFunction(method.get(), decl);
		}
		endScope();

		if (expr->supperClass != nullptr)
		{
			endScope();
		}

		return core::Value();
	}

	core::Value Resolver::visit(GetExpr* expr)
	{
		resolveStmt(expr->object.get());
		return core::Value();
	}

	core::Value Resolver::visit(SetExpr* expr)
	{
		resolveStmt(expr->value.get());
		resolveStmt(expr->object.get());
		return core::Value();

	}

	core::Value Resolver::visit(This* expr)
	{
		if (currentClass_ == ClassType::NONE)
		{
			logger_.write(core::LogMessage("Cannot use 'this' outside a class"));
		}
		resolveLockal(expr, expr->keyword);
		return core::Value();
	}

	core::Value Resolver::visit(Super* expr)
	{
		if (currentClass_ == ClassType::NONE)
		{
			logger_.write(core::LogMessage("Can`t use 'super' outside of class"));
		}
		else if (currentClass_ != ClassType::SUPERCLASS)
		{
			logger_.write(core::LogMessage("Can`t use supper in class with no suppercalass."));
		}
		

		resolveLockal(expr, expr->keyword);
		return core::Value();
	}

	void Resolver::resolveLockal(Expr* expr, Token name)
	{
		for (int i = static_cast<int>(scopes_.size() - 1); i >= 0; --i )
		{
			if (scopes_[i].find(name.lexeme) != scopes_[i].end())
			{
				interpreter_->resolve(expr, scopes_.size() - 1 - i);
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
			logger_.write(VariableAllradyMessage(name.lexeme));
		}
	}

	void Resolver::define(Token name)
	{
		if (scopes_.empty()) return;

		auto it = scopes_.back().find(name.lexeme);
		if (it != scopes_.back().end())
		{
			it->second =  true;
		}
		else
		{
			logger_.write(VariableAllradyMessage(name.lexeme));
		}
	}	

}
}