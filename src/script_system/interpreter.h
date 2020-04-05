#pragma once

#include "core/value.h"

#include "ast_visitor.h"

namespace script_system
{
	namespace parser {
		class Expr;

		class Interpreter : public AstVisitor
		{		
			public:
				core::Value visitLiteralExpr(Literal* expr);
				core::Value visitGroupingExpr(Grouping* expr);
				core::Value visitUnaryExpr(Unary* expr);
				core::Value visitBinaryExpr(Binary* expr);

				core::Value evaluate(Expr* expr);
		};
	}
}