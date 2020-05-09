#include "parser.h"
#include "core/logger.h"
#include "core/assert.h"


namespace script_system {
namespace parser {

    namespace {
		class ParserErrorMessage : public core::LogMessage
		{
		public:
			ParserErrorMessage(Token token, const string& message) 
				: token_(token), message_(message)
            {}

			virtual string toString() override
			{
				string result;
				result.append("token [");
                result.append(token_.toString());                
				result.append(" ] message = [");
                result.append(message_);
                result.append("]");

				return result;
			}

		private:
			Token token_;
			string message_;
		};
    }


	vector<ExprPtr> Parser::parse()
	{
        vector<ExprPtr> statements;
        while( !isAtEnd())
        {
            statements.push_back(declaration());
        }

		return statements;
	}

    ExprPtr Parser::declaration()
    {
        if (match({TokenType::CLASS})) return classDeclaration();
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return varDeclaration();

        return statement();
    }

    ExprPtr Parser::varDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expected variable name.");
        ExprPtr init = nullptr;
        if (match({TokenType::EQUAL}))
        {
            init = expression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
        return makeShared<Var>(name, init);
    }

    shared_ptr<Function> Parser::function(const string& kind)
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + "name.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name."); 
        vector<Token> parametrs;
        if (!check(TokenType::RIGHT_PAREN))
        {
            do
            {
                if (parametrs.size() > 24)
                {
                    error(peek(), "Cannot have more than 24 parametrs.");
                } 
                parametrs.push_back(consume(TokenType::IDENTIFIER, "Expect parametr name."));

            } while(match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parametrs.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
        auto body = block();
        return makeShared<Function>(name, parametrs, body);
    }

    ExprPtr Parser::classDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
        consume(TokenType::LEFT_BRACE, "Expected '{' before class body.");

        vector<shared_ptr<Function>> methods;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())        
        {
            methods.push_back(function("method"));
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after class body.");

        return makeShared<ClassExpr>(name, methods);
    }

    ExprPtr Parser::statement()
    {
        if (match({TokenType::FOR})) return forStatement();
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::PRINT})) return printStatement();
        if (match({TokenType::RETURN})) return returnStatement();
        if (match({TokenType::WHILE})) return whileStatement();
        if (match({TokenType::LEFT_BRACE})) return makeShared<Block>(block());

        return expressionStatement();
    }

    ExprPtr Parser::ifStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
        auto condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after 'if' condition.");

        auto thenBranch = statement();
        ExprPtr elseBranch = nullptr;
        if (match({TokenType::ELSE}))
        {
            elseBranch = statement();
        }

        return makeShared<IfExpr>(condition, thenBranch, elseBranch);
    }

    ExprPtr Parser::forStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'.");
        ExprPtr init = nullptr;
        if (match({TokenType::SEMICOLON}))
        {
            init = nullptr;
        } else if (match({TokenType::VAR}))
        {
            init = varDeclaration();
        }
        else
        {
            init = expressionStatement();
        }

        ExprPtr cond = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            cond = expression();
        }
        consume(TokenType::SEMICOLON, "Expected ';' after loop condition.");

        ExprPtr incr = nullptr;
        if (!check(TokenType::RIGHT_PAREN))
        {
            incr = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')' after for clauses.");

        ExprPtr body = statement();

        if (incr != nullptr)
        {
            body = makeShared<Block>(vector<ExprPtr>({body, makeShared<Stmt>(incr)}));
        }

        if (cond == nullptr) 
        {
            cond = makeShared<Literal>(true);
        }
        body = makeShared<WhileExpr>(cond, body);

        if (init != nullptr)
        {
            body = makeShared<Block>(vector<ExprPtr>({init, body}));
        }

        return body;        
    }

    vector<ExprPtr> Parser::block()
    {
        vector<ExprPtr> result;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            result.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
        return result;
    }

    ExprPtr Parser::printStatement()
    {
        auto value  = expression();
        consume(TokenType::SEMICOLON, "Expect ; after value");
        return makeShared<StmtPrint>(value);
    }

    ExprPtr Parser::returnStatement()
    {
        Token keyword = previous();
        ExprPtr value = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            value = expression();
        }

        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        return makeShared<Return>(keyword, value);
    }

    ExprPtr Parser::whileStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after value");
        ExprPtr condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after value");
        ExprPtr body = statement();

        return makeShared<WhileExpr>(condition, body);
    }

    ExprPtr Parser::expressionStatement()
    {
        auto expr  = expression();
        consume(TokenType::SEMICOLON, "Expect ; after value");
        return makeShared<Stmt>(expr);
    }

    ExprPtr Parser::expression()
    {
        return assignment();
    }

    ExprPtr Parser::assignment()
    {
        ExprPtr expr = logicOr();    

        if (match({TokenType::EQUAL}))
        {
            Token equals = previous();
            ExprPtr value = assignment();

            Variable* variable = dynamic_cast<Variable*>(expr.get());
            if (variable)
            {
                Token name = variable->name;
                return makeShared<Assign>(name, value);
            }
            else 
            {
                GetExpr* getExpr = dynamic_cast<GetExpr*>(expr.get());
                if (getExpr)
                {
                    return makeShared<SetExpr>(getExpr->object, getExpr->name, value);
                }
            }

            ASSERT(false, "Parser::assignmet");
        }
        return expr;
    }

    ExprPtr Parser::logicOr()
    {
        ExprPtr expr = logicAnd();

        if (match({TokenType::OR}))
        {
            Token opr = previous();
            ExprPtr right = logicAnd();
            expr = makeShared<Logical>(expr, opr, right);
        }

        return expr;
    }

    ExprPtr Parser::logicAnd()
    {
        ExprPtr expr = equality();

        while(match({TokenType::AND}))
        {
            Token opr = previous();
            ExprPtr right = equality();
            expr = makeShared<Logical>(expr, opr, right);
        }

        return expr;
    }


    ExprPtr Parser::equality()
    {
        auto expr = comparison();

        while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
        {
            Token opr = previous();
            auto right = comparison();
            expr = makeShared<Binary>(expr, opr, right);
        }

        return expr;
    }

    bool Parser::match(const vector<TokenType>& types)
    {
        for (auto type : types)
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }
        return false;
    }

    ExprPtr Parser::comparison()
    {
        auto expr = addition();

        while(match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
        {
            Token opr = previous();
            auto right = addition();
            expr = makeShared<Binary>(expr, opr, right);
        }

        return expr;
    }

    ExprPtr Parser::addition()
    {
        ExprPtr expr = multiplication();

        while(match({TokenType::MINUS, TokenType::PLUS}))
        {
            Token opr = previous();
            auto right = multiplication();
            expr = makeShared<Binary>(expr, opr, right);
        }

        return expr;
    }

    ExprPtr Parser::multiplication()
    {
        ExprPtr expr = unary();

        while(match({TokenType::SLASH, TokenType::STAR}))
        {
            Token opr = previous();
            auto right = unary();
            expr = makeShared<Binary>(expr, opr, right);
        }

        return expr;
    }

    ExprPtr Parser::unary()
    {
        if (match({TokenType::BANG, TokenType::MINUS}))
        {
            Token opr = previous();
            ExprPtr right = unary();
            return makeShared<Unary>(opr, right);
        }

        return call();
    }

    ExprPtr Parser::call()
    {
        ExprPtr expr = primary();

        while(true)
        {
            if (match({TokenType::LEFT_PAREN}))
            {
                expr = finishCall(expr);
            }
            else if (match({TokenType::DOT}))
            {
                auto name = consume(TokenType::IDENTIFIER, "Expect property name afte '.'.");
                expr = makeShared<GetExpr>(expr, name);
            }
            else
            {
                break;
            }            
        }
        
        return expr;
    }

    ExprPtr Parser::finishCall(const ExprPtr& expr)
    {
        vector<ExprPtr> arguments;
        if (!check(TokenType::RIGHT_PAREN))
        {
            do
            {   if (arguments.size() > 24)
                {
                    error(peek(), "Cannot have more 24 arguments.");
                }
                arguments.push_back(expression());
            } while (match({TokenType::COMMA}));
        }

        Token paren = consume(TokenType::RIGHT_PAREN, "Expext ')' after arguments.");
        return makeShared<Call>(expr, paren, arguments);
    }

    ExprPtr Parser::primary()
    {
        if (match({TokenType::FALSE})) return makeShared<Literal>(core::Value(false));
        if (match({TokenType::TRUE})) return makeShared<Literal>(core::Value(true));
        if (match({TokenType::NIL})) return makeShared<Literal>(core::Value());

		if (match({ TokenType::NUMBER })) return makeShared<Literal>(core::Value(std::stod(previous().lexeme)));
		if (match({TokenType::STRING})) return makeShared<Literal>(core::Value(previous().lexeme));

        if (match({TokenType::IDENTIFIER}))
            return makeShared<Variable>(previous());

        if (match({TokenType::LEFT_PAREN}))
        {
            ExprPtr expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return makeShared<Grouping>(expr);
        }
		error(peek(), "Expect expression.");
		return nullptr;
    }

    bool Parser::check(TokenType type) const 
    {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool Parser::isAtEnd() const 
    {
        return peek().type == TokenType::EndOF;
    }

    Token Parser::peek() const
    {
        return tokens_.at(current_);
    }

    Token Parser::previous() const 
    {
        return tokens_.at(current_ - 1);
    }

    Token Parser::advance()
    {
        if (!isAtEnd()) current_++;            
        return previous();
    }

    Token Parser::consume(TokenType type, const string& message)
    {
        if (check(type)) return advance();
        error(Token(type, "", core::Value(), -1), message);
        return Token(TokenType::EndOF, "", core::Value(), -1);
    }

    void Parser::error(Token token, const string& message)
    {
		logger_.write(ParserErrorMessage(token, message));	        
    }

}
}
