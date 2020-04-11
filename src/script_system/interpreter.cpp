#include <iostream>

#include "interpreter.h"

#include "ast.h"

namespace script_system{
    namespace parser {

		void Interpreter::interpret(const vector<ExprPtr>& exprs)
		{
            for (auto expr : exprs)
            {
                execute(expr.get());
            }			
		}

        void Interpreter::execute(Expr* expr)
        {
            evaluate(expr);
        }

        core::Value Interpreter::visit(Literal* expr)
        {
            return expr->value;
        }

        core::Value Interpreter::visit(Grouping* expr)
        {
            return evaluate(expr->expression.get());
        }

        core::Value Interpreter::visit(Unary* expr)
        {
            core::Value rigth = evaluate(expr->right.get());

            switch (expr->oper.type)
            {
            case TokenType::BANG:
                return !rigth.get<bool>();
            case TokenType::MINUS:
                return core::Value(-rigth.get<double>());
            default:
                break;
            }

            return core::Value();
        }

        core::Value Interpreter::visit(Binary* expr)
        {
            core::Value left = evaluate(expr->left.get());
            core::Value right = evaluate(expr->right.get());

            switch (expr->oper.type)
            {
            case TokenType::MINUS:
                return core::Value(left.get<double>() - right.get<double>());
            case TokenType::SLASH:
                return core::Value(left.get<double>() / right.get<double>());
            case TokenType::STAR:
                return core::Value(left.get<double>() * right.get<double>());
            case TokenType::PLUS:
				return left + right;
			case TokenType::GREATER:
				return core::Value(left.get<double>() > right.get<double>());
			case TokenType::GREATER_EQUAL:
				return core::Value(left.get<double>() >= right.get<double>());
			case TokenType::LESS:
				return core::Value(left.get<double>() < right.get<double>());
			case TokenType::LESS_EQUAL:
				return core::Value(left.get<double>() <= right.get<double>());
			case TokenType::BANG_EQUAL:
				return core::Value(!(left == right));
			case TokenType::EQUAL_EQUAL:
				return core::Value(left == right);
            default:
                break;
            }

            return core::Value();
        }

        core::Value Interpreter::visit(Stmt* expr)
        {
            evaluate(expr);            
            return core::Value();
        }

        core::Value Interpreter::visit(StmtPrint* expr)
        {
            auto val = evaluate(expr);            
            std::cout << val.to<string>() << std::endl;           
            return core::Value();
        }

        core::Value Interpreter::evaluate(Expr* expr)
        {
            return expr->accept(this);
        }
    }
}
 