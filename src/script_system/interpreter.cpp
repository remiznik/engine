#include "interpreter.h"

#include "ast.h"

namespace script_system{
    namespace parser {

        core::Value Interpreter::visitLiteralExpr(Literal* expr)
        {
            return expr->value;
        }

        core::Value Interpreter::visitGroupingExpr(Grouping* expr)
        {
            return evaluate(expr->expression.get());
        }

        core::Value Interpreter::visitUnaryExpr(Unary* expr)
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

        core::Value Interpreter::visitBinaryExpr(Binary* expr)
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
                return core::Value(left.get<double>() + right.get<double>());
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

        core::Value Interpreter::evaluate(Expr* expr)
        {
            return expr->accept(this);
        }
    }
}
 