#pragma once
#include <string>
#include <vector>

#include "token.h"

namespace script_system
{
	class Scanner
	{
		public:
			Scanner();

	private:
		std::string source_;
		std::vector<TokenType> tokens_;
	};
}
