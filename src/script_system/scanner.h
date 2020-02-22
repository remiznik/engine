#pragma once

#include "core/types.h"

#include "token.h"

namespace script_system
{
	class Scanner
	{
		public:
			Scanner(const string& source);

	private:
		string source_;
		vector<TokenType> tokens_;
	};
}
