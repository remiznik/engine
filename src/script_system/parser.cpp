#include "parser.h"
#include "core/logger.h"

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


	ExprPtr Parser::parse()
	{
		return expression();
	}

    ExprPtr Parser::expression()
    {
        return equality();
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
