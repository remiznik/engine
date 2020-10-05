#pragma once

#include "core/types.h"

#include "ast_visitor.h"


namespace script_system
{
	namespace parser {
		class Expr;

		class AstPrinter : public AstVisitor
		{
		public:
			AstPrinter();

			string print(const shared_ptr<Expr>& expr);

			virtual core::Value visit(class Binary* expr) override;
			virtual core::Value visit(class Grouping* expr) override;
			virtual core::Value visit(class Literal* expr) override;
			virtual core::Value visit(class Unary* expr) override;
		private:
			void parenthesize(const string& name, const vector<shared_ptr<Expr>>& exprs);
			void parenthesize(const vector<shared_ptr<Expr>>& exprs, const string& name);

			string result_;
		};
	}
}
