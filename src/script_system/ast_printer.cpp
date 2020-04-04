#include "ast_printer.h"
#include "ast.h"

namespace script_system {
	namespace parser {

		AstPrinter::AstPrinter()
		{
		}

		string AstPrinter::print(const shared_ptr<Expr>& expr)
		{
			expr->accept(this);
			return result_;
		}

		void AstPrinter::visit(Binary* expr)
		{
			parenthesize(expr->oper.lexeme, { expr->left, expr->right });
			//vector<shared_ptr<Expr>> exprs({ expr->left, expr->right });
			//parenthesize(exprs, expr->oper.lexeme);
		}

		void AstPrinter::parenthesize(const string & name, const vector<shared_ptr<Expr>>& exprs)
		{
			result_.append("(");
			result_.append(name);
			for (auto& expr : exprs)
			{
				result_.append(" ");
				expr->accept(this);
			}
			result_.append(")");
		}

		void AstPrinter::parenthesize(const vector<shared_ptr<Expr>>& exprs, const string & name)
		{

			for (auto& expr : exprs)
			{
				result_.append(" ");
				expr->accept(this);
			}
			result_.append(name);

		}

		void AstPrinter::visit(Grouping * expr)
		{
			parenthesize("group", { expr->expression });
		}

		void AstPrinter::visit(Literal * expr)
		{

			result_.append(expr->value.to<string>());
		}

		void AstPrinter::visit(Unary * expr)
		{
			parenthesize(expr->oper.lexeme, { expr->right });
		}

	}
}