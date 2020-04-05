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
			Binary(const shared_ptr<Expr>& l, Token o, const shared_ptr<Expr>& r)
				: left(l), oper(o), right(r)
			{

			}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			shared_ptr<Expr> left;
			Token oper;
			shared_ptr<Expr> right;

		};

		class Grouping : public Expr
		{
		public:
			Grouping(const shared_ptr<Expr>& e)
				: expression(e)
			{

			}
			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			shared_ptr<Expr> expression;
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
			Unary(Token o, const shared_ptr<Expr>& r)
				: right(r), oper(o)
			{

			}

			core::Value accept(AstVisitor* visitor) override
			{
				return visitor->visit(this);
			}

			Token oper;
			shared_ptr<Expr> right;
		};
	}
}