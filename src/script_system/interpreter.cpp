#include <iostream>

#include "core/m_assert.h"
#include "core/callabel.h"



#include "interpreter.h"
#include "ast.h"

namespace script_system{
namespace parser {

    class RetrunException : public std::exception
    {
    public:
        RetrunException(core::Value v)
            : value(v)
        {}
            
        core::Value value;
      };


    class Clock : public core::Callable
    {
    public:
        core::Value call(const vector<core::Value>& args) override
        {
            std::cout << " time " << std::endl;
            return core::Value();
        }
    };

    class Clamp : public core::Callable
    {
    public:
        core::Value call(const vector<core::Value>& args) override
        {
            auto x = args[0].get<double>();
            auto low = args[1].get<double>();
            auto high = args[2].get<double>();

            return core::Value(x < low ? low : (x > high ? high : x));            
        }
    };

    class InFunction : public core::Callable
    {
    public:
        InFunction(Interpreter* inter, Function* expr, const shared_ptr<Environment>& closure)
            : inter_(inter), expr_(expr), closure_(closure)
        {}

        core::Value call(const vector<core::Value>& args) override
        {
            auto env = makeShared<Environment>(closure_);
            ASSERT(args.size() == expr_->params.size(), "Not equal arguments .")
                
            for (size_t i = 0; i < expr_->params.size(); ++i)
            {
                env->define(expr_->params[i].lexeme, args[i]);
            }
            try
            {
                inter_->execute(expr_->body, env);
            }
            catch(const RetrunException& e)
            {
                //std::cout << " catch " << std::endl;
                return e.value;
            }

            return core::Value();
        }
    private:
        Interpreter* inter_;
        Function* expr_;
        shared_ptr<Environment> closure_;
    };

    Interpreter::Interpreter()
    {
        environment_ = makeShared<Environment>();
        registreFunction("clamp", makeShared<Clamp>());
    }
	void Interpreter::interpret(const vector<ExprPtr>& exprs)
	{
        expresions_ = exprs;
        for (auto expr : expresions_)
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
			return core::Value(left.get<double>()+ right.get<double>());
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
        return environment_->get(expr->name.lexeme);
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
        auto clock = static_cast<core::Callable*>(obj.get());

        return clock->call(arguments);
    }

    core::Value Interpreter::visit(Function* expr)
    {
        auto fnc = makeShared<InFunction>(this, expr, environment_);
        environment_->define(expr->name.lexeme, core::Value(fnc));
        return core::Value();
    }

    core::Value Interpreter::visit(Return* expr)
    {
        core::Value val;            
        if (expr->value != nullptr) val = evaluate(expr->value.get());
            
        throw RetrunException(val);
        return core::Value();
    }
		

    void Interpreter::execute(const vector<ExprPtr>& statements, const shared_ptr<Environment>& env)
    {
		// becaus return realize by exception (
		ScopeGuard<shared_ptr<Environment>> guard(environment_);
        environment_ = env;
            
        for (auto& expr : statements)
        {
            execute(expr.get());
        }            
    }

    void Interpreter::update(int s, int x, int y)
    {
        auto fnc = environment_->get("update");
         
        shared_ptr<core::Object> obj = fnc.get<shared_ptr<core::Object>>();
        auto clock = static_cast<core::Callable*>(obj.get());
        if (clock)
        {
            clock->call({core::Value(double(s)), core::Value(double(x)), core::Value(double(y))});
        }       
    }

    void Interpreter::registreFunction(const string& name, const shared_ptr<core::Callable>& fnc)
    {
        environment_->define(name, core::Value(fnc));
    }
}
}
 