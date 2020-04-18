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
            {
                
            }

			virtual string toString() override
			{
				string result;
				result.append("token ");
				result.append(" message = [");
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

    ExprPtr Parser::statement()
    {
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::PRINT})) return printStatement();
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
        ExprPtr expr = equality();

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
            std::cout << equals.toString() << std::endl;
            ASSERT(false, "Parser::assignmet");
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

        return primary();
    }

    ExprPtr Parser::primary()
    {
        if (match({TokenType::FALSE})) return makeShared<Literal>(core::Value(false));
        if (match({TokenType::TRUE})) return makeShared<Literal>(core::Value(true));
        if (match({TokenType::NIL})) return makeShared<Literal>(core::Value());

        if (match({TokenType::NUMBER, TokenType::STRING})) return makeShared<Literal>(core::Value(previous().lexeme));

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
        return Token(TokenType::EndOF, "", core::Value(), -1);
    }

    void Parser::error(Token token, const string& message)
    {
        auto msg = ParserErrorMessage(token, message);
		logger_.write(msg);	        
    }

}
}
