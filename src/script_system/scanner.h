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

		vector<Token> scan(const string& source);

	private:
		bool isEnd();
		void scanToken();
		void addToken(TokenType type);
		void addToken(TokenType type, core::Value value);
		char advance();

	private:
		core::Logger& loggre_;
		string source_;
		vector<Token> tokens_;

		uint start_ {0};
		uint current_{0};
		uint line_ {1};
	};
}
