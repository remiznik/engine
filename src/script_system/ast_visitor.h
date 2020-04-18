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
			virtual core::Value visit(class Stmt* expr) = 0;
			virtual core::Value visit(class StmtPrint* expr) = 0;
			virtual core::Value visit(class Var* expr) = 0;
			virtual core::Value visit(class Variable* expr) = 0;
			virtual core::Value visit(class Assign* expr) = 0;
			virtual core::Value visit(class Block* expr) = 0;


		};
	}
}
