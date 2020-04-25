#include <iostream>

#include "core/object.h"

#include "interpreter.h"
#include "ast.h"

namespace script_system{
    namespace parser {

        class Callable : public core::Object
        {
        public:
            virtual core::Value call(const vector<core::Value>& args) = 0;
        };

        class Clock : public Callable
        {
        public:
            core::Value call(const vector<core::Value>& args) override
            {
                std::cout << " time " << std::endl;
                return core::Value();
            }
        };

        class InFunction : public Callable
        {
        public:
            InFunction(Interpreter* inter, Function* expr)
                : inter_(inter), expr_(expr)
            {}

            core::Value call(const vector<core::Value>& args) override
            {
                auto env = makeShared<Environment>(inter_->environment());
                for (size_t i = 0; i < expr_->params.size(); ++i)
                {
                    env->define(expr_->params[i].lexeme, args[i]);
                }
                inter_->execute(expr_->body, env);

                return core::Value();
            }
        private:
            Interpreter* inter_;
            Function* expr_;
        };

        Interpreter::Interpreter()
        {
            environment_ = makeShared<Environment>();
        }
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
				return left.get<double>() + right.get<double>();
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
            evaluate(expr->expr.get());            
            return core::Value();
        }

        core::Value Interpreter::visit(StmtPrint* expr)
        {
            auto val = evaluate(expr->print.get());            
            std::cout << val.to<string>() << std::endl;
            return core::Value();
        }

        core::Value Interpreter::visit(Var* expr)
        {
            core::Value value;
            if (expr->initilize != nullptr)
            {
                core::Value value = evaluate(expr->initilize.get());
                environment_->define(expr->name.lexeme, value);
            }
            else
            {
                environment_->define(expr->name.lexeme, core::Value());
            }            
            
            return core::Value();
        }

        core::Value Interpreter::visit(Variable* expr)
        {
            return environment_->get(expr->name);
        }

        core::Value Interpreter::visit(Assign* expr)
        {
            core::Value val = evaluate(expr->value.get());

            environment_->assign(expr->name, val);

            return val;
        }

        core::Value Interpreter::evaluate(Expr* expr)
        {
            return expr->accept(this);
        }

        core::Value Interpreter::visit(Block* expr)
        {
            execute(expr->statements, makeShared<Environment>(environment_));
            return core::Value();
        }

        core::Value Interpreter::visit(IfExpr* expr)
        {
            auto con = evaluate(expr->condition.get());
            if (con.get<bool>())
            {
                execute(expr->thenBranch.get());
            }
            else if (expr->elseBranch) 
            {
                execute(expr->elseBranch.get());
            }
            return core::Value();
        }

        core::Value Interpreter::visit(Logical* expr)
        {
            auto left = evaluate(expr->left.get());

            if (expr->opr.type == TokenType::OR)
            {
                if (left.get<bool>()) return left;                
            }
            else 
            {
                if (!left.get<bool>()) return left;
            }

            return evaluate(expr->right.get());

        }

        core::Value Interpreter::visit(WhileExpr* expr)
        {
            while (evaluate(expr->condition.get()).get<bool>())
            {
                execute(expr->body.get()); 
            }
            
            return core::Value();
        }

        core::Value Interpreter::visit(Call* expr)
        {
            auto callee = evaluate(expr->callee.get());

            vector<core::Value> arguments;
            for (auto argument : expr->arguments)
            {
                arguments.push_back(evaluate(argument.get()));
            }

            shared_ptr<core::Object> obj = callee.get<shared_ptr<core::Object>>();
            auto clock = static_cast<Callable*>(obj.get());

            return clock->call(arguments);
        }

        core::Value Interpreter::visit(Function* expr)
        {
            auto fnc = makeShared<InFunction>(this, expr);
            environment_->define(expr->name.lexeme, core::Value(fnc));
            return core::Value();
        }

        void Interpreter::execute(const vector<ExprPtr>& statements, const shared_ptr<Environment>& env)
        {
            auto previos = environment_;
            environment_ = env;
            
            for (auto& expr : statements)
            {
                execute(expr.get());
            }

            environment_ = previos;
        }
    }
}
 