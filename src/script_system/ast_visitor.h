#pragma once

#include "core/types.h"
#include "core/value.h"

namespace script_system {
	namespace parser {
		class AstVisitor
		{
		public:
			virtual core::Value visit(class Binary* expr) = 0;
			virtual core::Value visit(class Grouping* expr) = 0;
			virtual core::Value visit(class Literal* expr) = 0;
			virtual core::Value visit(class Unary* expr) = 0;
		};
	}
}