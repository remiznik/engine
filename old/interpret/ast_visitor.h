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
			virtual core::Value visit(class IfExpr* expr) = 0;
			virtual core::Value visit(class Logical* expr) = 0;
			virtual core::Value visit(class WhileExpr* expr) = 0;
			virtual core::Value visit(class Call* expr) = 0;
			virtual core::Value visit(class Function* expr) = 0;
			virtual core::Value visit(class Return* expr) = 0;
			virtual core::Value visit(class ClassExpr* expr) = 0;
			virtual core::Value visit(class GetExpr* expr) = 0;
			virtual core::Value visit(class SetExpr* expr) = 0;
			virtual core::Value visit(class This* expr) = 0;
			virtual core::Value visit(class Super* expr) = 0;
		};
	}
}
