#pragma once

#include "core/types.h"
#include "core/logger.h"

#include "token.h"
#include "ast.h"

namespace script_system
{
	namespace parser {

		class Parser
		{
		public:
			Parser(core::Logger& logger, const vector<Token>& tokens)
				 : logger_(logger), tokens_(tokens)
			{}

			vector<ExprPtr> parse();

		private:
			ExprPtr assignment();
			ExprPtr logicOr();
			ExprPtr logicAnd();
			ExprPtr	varDeclaration();
			ExprPtr declaration();
			ExprPtr expressionStatement();
			ExprPtr printStatement();
			ExprPtr returnStatement();
			ExprPtr whileStatement();
			ExprPtr statement();
			ExprPtr expression();
			ExprPtr equality();
			ExprPtr comparison();
			ExprPtr addition();
			ExprPtr multiplication();
			ExprPtr unary();
			ExprPtr primary();
			ExprPtr ifStatement();
			ExprPtr forStatement();
			ExprPtr call();
			ExprPtr finishCall(const ExprPtr& expr);
			shared_ptr<Function> function(const string& kind);
			ExprPtr classDeclaration();
			vector<ExprPtr> block();

			bool match(const vector<TokenType>& types);
			bool check(TokenType type) const;
			bool isAtEnd() const;

			Token peek() const;
			Token previous() const;
			Token advance();
			Token consume(TokenType type, const string& message);

			void error(Token token, const string& message);

		private:
			core::Logger& logger_;
			vector<Token> tokens_;
			int current_{0};
		};
	}
}