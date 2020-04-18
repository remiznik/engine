#pragma once

#include "core/types.h"
#include "core/value.h"

#include "ast_visitor.h"
#include "token.h"


namespace script_system
{
	namespace parser {

		class Expr
		{
		public:
			virtual core::Value accept(AstVisitor* visitor) = 0;
		};

		using ExprPtr = shared_ptr<Expr>;

		class Binary : public Expr
		{
		public:
			Binary(const ExprPtr& l, Token o, const ExprPtr& r)
				: left(l), oper(o), right(r)
			{

			}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			ExprPtr left {nullptr};
			Token oper;
			ExprPtr right {nullptr};

		};

		class Grouping : public Expr
		{
		public:
			Grouping(const ExprPtr& e)
				: expression(e)
			{

			}
			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			ExprPtr expression {nullptr};
		};

		class Literal : public Expr
		{
		public:
			Literal(core::Value v)
				: value(v)
			{

			}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			core::Value value;
		};

		class Unary : public Expr
		{
		public:
			Unary(Token o, const ExprPtr& r)
				: right(r), oper(o)
			{

			}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			Token oper;
			ExprPtr right {nullptr};
		};

		class  Stmt : public Expr
		{
		public:
			Stmt(const ExprPtr& e)				
				: expr(e)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			ExprPtr expr {nullptr};
		};

		class  StmtPrint : public Expr
		{
		public:
			StmtPrint(const ExprPtr& p) 
				: print(p)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}
			
			ExprPtr print {nullptr};
		};

		class Var : public Expr
		{
		public:
			Var(Token n, const ExprPtr& i)
				: name(n), initilize(i)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}
		
			Token name;
			ExprPtr initilize {nullptr};
		};

		class Variable : public Expr
		{
		public:
			Variable(Token n)
				: name(n)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			Token name;
		};

		class Assign : public Expr
		{
		public:
			Assign(Token n, const ExprPtr& v)
				: name(n), value(v)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}
			
			Token name;
			ExprPtr value {nullptr};
		};

		class Block : public Expr
		{
		public:
			Block(const vector<ExprPtr> s)
				: statements(s)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}


			vector<ExprPtr> statements;
		};

		class IfExpr : public Expr
		{
		public:
			IfExpr(const ExprPtr& c, const ExprPtr& t, const ExprPtr& e)
				: condition(c), thenBranch(t), elseBranch(e)
			{}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			ExprPtr condition {nullptr};
			ExprPtr thenBranch {nullptr};
			ExprPtr elseBranch {nullptr};
		};

		class Logical : public Expr
		{
		public:
		};

	}
}