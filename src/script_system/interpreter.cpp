#include <iostream>

#include "core/m_assert.h"
#include "core/callabel.h"



#include "interpreter.h"
#include "class_def.h"
#include "ast.h"

namespace script_system{
namespace parser {
    class Clock : public Callable
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

        virtual string toString() const override
        {
            return "Clamp";
        }
    };
    

Interpreter::Interpreter()
{
    globals_ = makeShared<Environment>();    
    environment_ = globals_;

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
    return lookUpVariable(expr->name, expr);
}

core::Value Interpreter::visit(Assign* expr)
{
    core::Value val = evaluate(expr->value.get());

    auto it = locals_.find(expr);
    if (it != locals_.end())
    {
        environment_->assignAt(it->second, expr->name, val);
    }
    else
    {
        globals_->assign(expr->name, val);
    }
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
    auto fnc = makeShared<InFunction>(this, expr, environment_, false);
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

core::Value Interpreter::visit(ClassExpr* expr)
{
    environment_->define(expr->name.lexeme, core::Value());

    map<string, shared_ptr<InFunction>> methods;
    for (auto method : expr->methods)
    {
        methods.emplace(method->name.lexeme, makeShared<InFunction>(this, method.get(), environment_, string(method->name.lexeme) == "init"));
    }

    auto inClass = makeShared<InClass>(expr->name.lexeme, methods);
    environment_->assign(expr->name, core::Value(inClass));
    return core::Value();
}

core::Value Interpreter::visit(GetExpr* expr)
{
    auto objValue = evaluate(expr->object.get());
    auto obj = objValue.get<shared_ptr<core::Object>>();
    auto inst = static_cast<InClassInstance*>(obj.get());
    if (inst)
    {
        return inst->get(expr->name.lexeme);
    }

    ASSERT(false, "Only instance have property ");
    return core::Value();
}

core::Value Interpreter::visit(SetExpr* expr)
{
    auto objValue = evaluate(expr->object.get());
    auto obj = objValue.get<shared_ptr<core::Object>>();
    auto inst = static_cast<InClassInstance*>(obj.get());
    if (inst == nullptr)
    {
        ASSERT(false, "Only instance have field. ");
        return core::Value();
    }
    auto val = evaluate(expr->value.get());
    inst->set(expr->name.lexeme, val);
    return val;
}

core::Value Interpreter::visit(This* expr)
{
    return lookUpVariable(expr->keyword, expr);
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

void Interpreter::resolve(Expr* expr, int depth)
{
    locals_.emplace(expr, depth);
}

core::Value Interpreter::lookUpVariable(Token name, Expr* expr)
{
    auto it = locals_.find(expr);
    if (it != locals_.end())
    {        
        return environment_->getAt(it->second, name.lexeme);
    }
    
    return globals_->get(name.lexeme);    
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
 