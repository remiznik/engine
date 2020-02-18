#include "token.h"

namespace script_system
{
	Token::Token(TokenType type, const std::string& lexeme, int line)
		: type(type), lexeme(lexeme), line(line)
	{
	}
}
