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
		default:
		{
			auto msg = ScannerErrorMessage(string("Unexpecte characte"), line_);
			loggre_.write(msg);
			break;
		}

		}

	}
}
