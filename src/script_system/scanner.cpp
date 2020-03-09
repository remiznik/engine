#include "scanner.h"

namespace script_system
{
	namespace {
		class ScannerErrorMessage : public core::LogMessage
		{
		public:
			ScannerErrorMessage(const string& text, int line) 
				: text_(text), line_(line) {}

			virtual string toString() override
			{
				string result;
				result.append(text_);
				result.append(" line ");
				result.append(std::to_string(line_));
				return result;
			}

		private:
			string text_;
			int line_;
		};

	}

	Scanner::Scanner(core::Logger& logger)
		: loggre_(logger)
	{}

	vector<Token> Scanner::scan(const string& source)
	{
		while(!isEnd())
		{
			start_ = current_;
			scanToken();			
		}

		tokens_.push_back(Token(TokenType::EndOF, "", core::Value(), line_));
		return tokens_;
	}

	// private 
	bool Scanner::isEnd()
	{
		return current_ >= source_.size();
	}
	
	bool Scanner::isDigit(char c) const 
	{
		return c >= '0' && c <= '9';
	}

	char Scanner::advance()
	{
		current_++;
		return source_[current_ - 1];
	}

	void Scanner::addToken(TokenType type)
	{
		addToken(type, core::Value());
	}

	void Scanner::addToken(TokenType type, core::Value value)
	{
		auto lexeme = source_.substr(start_, current_);
		tokens_.push_back(Token(type, lexeme, value, line_));
	}

	bool Scanner::match(char expected)
	{
		if (isEnd()) return false;
		if (source_[current_] != expected) return false;

		current_++;
		return true;
	}

	char Scanner::peek()
	{
		if (isEnd()) return '\0';
		return source_[current_];
	}

	void Scanner::getNumber()
	{
		while(isDigit(peek())) advance();

		if (peek() == '.' && isDigit(peekNext()))
		{
			advance();

			while(isDigit(peek())) advance();
		}

		auto text = source_.substr(start_, current_);
		addToken(TokenType::NUMBER, core::Value(std::stod(text)))
	}

	void Scanner::getString()
	{
		while(peek() != '"' && !isEnd()) {
			if (peek() == '\n') line_++;
			advance();
		}

		if (isEnd())
		{
			auto msg = ScannerErrorMessage(string("Untermnated string"), line_);
			loggre_.write(msg);	
			return;
		}

		advance();

		auto text = source_.substr(start_ + 1, current_ + 1);
		addToken(TokenType::STRING, core::Value(text));
	}

	void Scanner::scanToken()
	{
		char c = advance();
		switch (c)
		{
			case '(': addToken(TokenType::LEFT_PAREN); break;
			case ')': addToken(TokenType::RIGHT_PAREN); break;
			case '{': addToken(TokenType::LEFT_BRACE); break;
			case '}': addToken(TokenType::RIGHT_BRACE); break;
			case ',': addToken(TokenType::COMMA); break;
			case '.': addToken(TokenType::DOT); break;
			case '-': addToken(TokenType::MINUS); break;		
			case '+': addToken(TokenType::PLUS); break;
			case ';': addToken(TokenType::SEMICOLON); break;
			case '*': addToken(TokenType::STAR); break;
			case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
			case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
			case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
			case '>': addToken(match('>') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
			case '/':
			{
				if (match('/'))
				{
					// comment gose until end of line
					while(peek() != '\n' && !isEnd()) advance();
				}
				else
				{
					addToken(TokenType::SLASH);
				}			
			}
			case ' ':
			case '\r':
			case '\t':
				break;
			case '\n':
				line_++;
				break;
			case '"': getString(); break;

			default:
			{
				if (isDigit(c))
				{
					getNumber();
				}
				else
				{
					auto msg = ScannerErrorMessage(string("Unexpecte characte"), line_);
					loggre_.write(msg);
				}
				break;
			}
		}

	}
}
