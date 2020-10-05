#pragma once

#include "core/types.h"
#include "core/value.h"
#include "core/logger.h"

#include "token.h"

namespace script_system
{
	
	class Scanner
	{
	public:
		Scanner(core::Logger& logger);

		vector<parser::Token> scan(const string& source);

	private:
		bool isEnd();
		bool match(char expected);
		bool isDigit(char c) const;
		bool isAlpha(char c) const;
		bool isAlphaNumeric(char c) const;
		char peek();
		char peekNext();
		void getNumber();
		void getString();
		void scanToken();
		void identifier();
		void addToken(parser::TokenType type);
		void addToken(parser::TokenType type, core::Value value);
		char advance();


	private:
		core::Logger& loggre_;
		string source_;
		vector<parser::Token> tokens_;

		int start_ {0};
		int current_{0};
		int line_ {1};
	};
}
