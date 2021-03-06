#include "vm/compiler.h"
#include "vm/scanner.h"

#include "vm/chunk.h"
#include "vm/debug.h"

#include "vm/common.h"
#include "vm/value.h"

#include "vm/object.h"

#include "vm/memory.h"

#include "core/types.h"

#include <stdio.h>
#include <cstdlib>

namespace script_system {
namespace vm {

    typedef enum {
        TYPE_FUNCTION,
        TYPE_METHOD,
        TYPE_INITIALIZER,
        TYPE_SCRIPT
    } FunctionType;

namespace {

  void grouping(bool);
  void dot(bool);
  void this_(bool);
  void super_(bool);
  void unary(bool);
  void binary(bool);
  void number(bool);
  void literal(bool);
  void string(bool);
  void variable(bool);
  void and_(bool);
  void or_(bool);
  void call(bool);
  void printStatement();
  void statement();
  void expressionStatement();
  void varDeclaration();
  void declareVariable();
  bool identifiersEqual(Token* a, Token* b);
  void addLocal(Token name);
  void beginScope();
  void endScope();
  void block();
  void ifStatement();
  void returnStatment();
  void whileStatement();
  void forStatement();
  void switchStatement();
  void funDeclaration();
  void function(FunctionType);
  void markInitialized();
  uint8_t argumentList();

  uint8_t parseVariable(const char* message);
  uint8_t identifierConstant(Token* name);
  void defineVariable(uint8_t global);
  void namedVariable(Token name, bool);



  typedef enum {                  
    PREC_NONE,                    
    PREC_ASSIGNMENT,  // =        
    PREC_OR,          // or       
    PREC_AND,         // and      
    PREC_EQUALITY,    // == !=    
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -      
    PREC_FACTOR,      // * /      
    PREC_UNARY,       // ! -      
    PREC_CALL,        // . ()     
    PREC_PRIMARY                  
  } Precedence;

  typedef struct {       
    Token current;       
    Token previous;
    bool hadError; 
    bool panicMode;

  } Parser; 

  Parser parser;  

  typedef void (*ParseFn)(bool);

  typedef struct {        
    ParseFn prefix;       
    ParseFn infix;        
    Precedence precedence;
  } ParseRule;

  typedef struct {
      Token name;
      int depth;
      bool isCaptured;
  } Local;

  typedef struct {
      uint8_t index;
      bool isLocal;
  } Upvalue;

  struct Compiler {
      struct Compiler* enclosing;
      ObjFunction* function;
      FunctionType type;

      Local locals[UINT8_COUNT];
      Upvalue upvalues[UINT8_COUNT];
      int localCount;
      int scopeDepth;
  };

  struct ClassCompiler
  {
      struct ClassCompiler* enclosing;
      Token name;
      bool hasSuperclass;
  };

  Compiler* current = nullptr;
  ClassCompiler* currentClass = nullptr;

  void initCompiler(Compiler* compiler, FunctionType type)
  {
    compiler->enclosing = current;
    compiler->function = nullptr;
    compiler->type = type;

    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    compiler->function = newFunction();
    current = compiler;

    if (type != TYPE_SCRIPT)
    {
        current->function->name = copyString(parser.previous.start, parser.previous.length);
    }

    Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->isCaptured = false;
    if (type != TYPE_FUNCTION)
    {
        local->name.start = "this";
        local->name.length = 4;
    }
    else
    {
        local->name.start = "";
        local->name.length = 0;
    }
  }


  ParseRule rules[] = {                                              
    { grouping, call,    PREC_CALL},       // TOKEN_LEFT_PAREN      
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_RIGHT_PAREN     
    { nullptr,     nullptr,    PREC_NONE },      // TOKEN_LEFT_BRACE
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_RIGHT_BRACE     
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_COMMA           
    { nullptr,     dot,    PREC_CALL},       // TOKEN_DOT             
    { unary,    binary,  PREC_TERM },       // TOKEN_MINUS           
    { nullptr,     binary,  PREC_TERM },       // TOKEN_PLUS            
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_SEMICOLON     
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_COLON     
    { nullptr,     binary,  PREC_FACTOR },     // TOKEN_SLASH           
    { nullptr,     binary,  PREC_FACTOR },     // TOKEN_STAR            
    { unary,    nullptr,    PREC_NONE },       // TOKEN_BANG            
    { nullptr,     binary,  PREC_EQUALITY },   // TOKEN_BANG_EQUAL      
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_EQUAL           
    { nullptr,     binary,  PREC_EQUALITY },   // TOKEN_EQUAL_EQUAL     
    { nullptr,     binary,  PREC_COMPARISON }, // TOKEN_GREATER         
    { nullptr,     binary,  PREC_COMPARISON }, // TOKEN_GREATER_EQUAL   
    { nullptr,     binary,  PREC_COMPARISON }, // TOKEN_LESS            
    { nullptr,     binary,  PREC_COMPARISON }, // TOKEN_LESS_EQUAL      
    { variable,   nullptr,    PREC_NONE },       // TOKEN_IDENTIFIER      
    { string,     nullptr,    PREC_NONE },       // TOKEN_STRING          
    { number,     nullptr,    PREC_NONE },       // TOKEN_NUMBER          
    { nullptr,     and_,    PREC_AND },       // TOKEN_AND             
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_CLASS           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_ELSE            
    { literal,  nullptr,    PREC_NONE },       // TOKEN_FALSE           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_FOR             
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_FUN             
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_IF              
    { literal,  nullptr,    PREC_NONE },       // TOKEN_NIL             
    { nullptr,     or_,    PREC_OR},       // TOKEN_OR              
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_PRINT           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_RETURN          
    { super_,     nullptr,    PREC_NONE },       // TOKEN_SUPER           
    { this_,     nullptr,    PREC_NONE },       // TOKEN_THIS            
    { literal,  nullptr,    PREC_NONE },       // TOKEN_TRUE            
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_VAR             
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_WHILE           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_SWITCH
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_CASE
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_DEFAULT
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_ERROR           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_EOF             
  };

  ParseRule* getRule(TokenType type) 
  {
    return &rules[type];                     
  }                                          
 

  Chunk* currentChunk()
  {
    return &current->function->chunk;
  }

  void errorAt(Token* token, const char* message) 
  {
    if (parser.panicMode) return;
    parser.panicMode = true;


    fprintf(stderr, "[line %d] Error", token->line);             

    if (token->type == TOKEN_EOF) {                              
      fprintf(stderr, " at end");                                
    } else if (token->type == TOKEN_ERROR) {                     
      // Nothing.                                                
    } else {                                                     
      fprintf(stderr, " at '%.*s'", token->length, token->start);
    }                                                            

    fprintf(stderr, ": %s\n", message);                          
    parser.hadError = true;                                      
  }  

  void errorAtCurrent(const char* message) 
  {
    errorAt(&parser.current, message);             
  }  

  void error(const char* message) 
  {
    errorAt(&parser.previous, message);
  }

  void advance()
  {
    parser.previous = parser.current;

    for(;;){
      parser.current = scanToken();
      if (parser.current.type != TOKEN_ERROR) break;

      errorAtCurrent(parser.current.start);
    }
  }

  void consume(TokenType type,  const char* message)
  {
    if (parser.current.type == type)
    {
      advance();
      return;
    }
    errorAtCurrent(message);
  }

  bool check(TokenType type)
  {
    return parser.current.type == type;
  }

  bool match(TokenType type)
  {
    if (!check(type)) return false;
    advance();
    return true;
  }

  void emitByte(uint8_t byte)
  {
    writeChunk(currentChunk(), byte, parser.previous.line);
  }

  void emitBytes(uint8_t one, uint8_t two)
  {
    emitByte(one);
    emitByte(two);
  }

  uint8_t makeConstant(Value value)
  {
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
      error("To many constate in one chunk");      
      return 0;
    }

    return static_cast<uint8_t>(constant);
  }

  void emitConstant(Value value)
  {
    emitBytes(OP_CONSTANT, makeConstant(value));
  }

  void emitReturn()
  {
    if (current->type == TYPE_INITIALIZER)
    {
        emitBytes(OP_GET_LOCAL, 0);
    }
    else
    {
        emitByte(OP_NIL);
    }
    
    emitByte(OP_RETURN);
  }

  ObjFunction* endCompiler()
  {
    emitReturn();
    ObjFunction* function = current->function;
#ifdef DEBUG_PRINT_CODE                      
    if (!parser.hadError) 
    {                    
      disassembleChunk(currentChunk(), function->name != nullptr ? function->name->chars : "<script>");
    }                                          
#endif               
    current = current->enclosing;
    return function;
  }

  void parsePrecedence(Precedence precedence)
  {
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == nullptr)
    {
      error("Expect expression.");
      return;
    }

    bool canAssigment = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssigment);

    while (precedence <= getRule(parser.current.type)->precedence)
    {
      advance();
      ParseFn infixRule = getRule(parser.previous.type)->infix;
      infixRule(canAssigment);
    }

    if (canAssigment && match(TOKEN_EQUAL))
    {
        error("Invalid assigment target.");
    }
  }

  void expression()
  {
    parsePrecedence(PREC_ASSIGNMENT);
  }

  void synchronize()
  {
    parser.panicMode = false;

    while(parser.current.type != TOKEN_EOF)
    {
      if (parser.previous.type == TOKEN_SEMICOLON) return;

      switch(parser.current.type)
      {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
          return;
        default:
        ;
      }
      advance();
    }
  }

  void method()
  {
      consume(TOKEN_IDENTIFIER, "Expect method name.");
      uint8_t constant = identifierConstant(&parser.previous);
      FunctionType type = TYPE_METHOD;
      if (parser.previous.length == 4 && memcmp(parser.previous.start, "init", 4) == 0)
      {
          type = TYPE_INITIALIZER;
      }
      function(type);
      emitBytes(OP_METHOD, constant);
  }

  Token syntheticToken(const char* text)
  {
      Token token;
      token.start = text;
      token.length = (int)strlen(text);
      return token;
  }

  void classDeclaration()
  {
      consume(TOKEN_IDENTIFIER, "Expect class name.");
      Token className = parser.previous;
      uint8_t nameConstant = identifierConstant(&parser.previous);
      declareVariable();


      emitBytes(OP_CLASS, nameConstant);
      defineVariable(nameConstant);

      ClassCompiler classCompiler;
      classCompiler.name = parser.previous;
      classCompiler.enclosing = currentClass;
      classCompiler.hasSuperclass = false;
      currentClass = &classCompiler;

      if (match(TOKEN_LESS))
      {
          consume(TOKEN_IDENTIFIER, "Expect supperclass name.");
          variable(false);
          if (identifiersEqual(&className, &parser.previous))
          {
              error("A class can`t inherit form itself.");
          }

          beginScope();
          addLocal(syntheticToken("super"));
          defineVariable(0);

          namedVariable(className, false);
          emitByte(OP_INHERIT);
          classCompiler.hasSuperclass = true;
      }


      namedVariable(className, false);
      consume(TOKEN_LEFT_BRACE, "Expect '{' before class body.");
      while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
      {
          method();
      }
      consume(TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
      emitByte(OP_POP);

      if (classCompiler.hasSuperclass)
      {
          endScope();
      }

      currentClass = currentClass->enclosing;
  }

  void declaration()
  {
    if (match(TOKEN_CLASS))
    {
        classDeclaration();
    }
    else if (match(TOKEN_FUN))
    {
        funDeclaration();
    }
    else if (match(TOKEN_VAR))
    {
      varDeclaration();
    }
    else
    {
      statement();
    }


    if (parser.panicMode) synchronize();
  }

  void funDeclaration()
  {
      uint8_t global = parseVariable("Expect function name.");
      markInitialized();
      function(TYPE_FUNCTION);
      defineVariable(global);
  }

  void varDeclaration()
  {
    uint8_t global = parseVariable("Expect variable name");

    if (match(TOKEN_EQUAL))
    {
      expression();
    }
    else 
    {
      emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

    defineVariable(global);
  }



  uint8_t parseVariable(const char* message)
  {
    consume(TOKEN_IDENTIFIER, message );

    declareVariable();
    if (current->scopeDepth > 0) return 0;

    return identifierConstant(&parser.previous);
  }

  uint8_t identifierConstant(Token* name)
  {
    return makeConstant(OBJ_VAL(copyString(name->start, name->length))); 
  }

  void declareVariable()
  {
      if (current->scopeDepth == 0) return;

      Token* name = &parser.previous;
      for (int i = current->localCount - 1; i >= 0; i--)
      {
          Local* local = &current->locals[i];
          if (local->depth != -1 && local->depth < current->scopeDepth)
          {
              break;
          }

          if (identifiersEqual(name, &local->name))
          {
              error("Already variable with this name in this scope");
          }
      }
      addLocal(*name);
  }

  bool identifiersEqual(Token* a, Token* b)
  {
      if (a->length != b->length) return false;
      return memcmp(a->start, b->start, a->length) == 0;
  }

  void addLocal(Token name)
  {
      if (current->localCount == UINT8_COUNT)
      {
          error("Too many locak variable in function.");
          return;
      }

      Local* local = &current->locals[current->localCount++];
      local->name = name;
      local->depth = -1;
      local->isCaptured = false;
  }

  void markInitialized()
  {
    if (current->scopeDepth == 0) return;
    current->locals[current->localCount - 1].depth = current->scopeDepth;
  }

  void defineVariable(uint8_t global)
  {
    if (current->scopeDepth > 0)
    {
        markInitialized();
        return;
    }

    emitBytes(OP_DEFINE_GLOBAL, global);
  }

  void statement()
  {
    if(match(TOKEN_PRINT))
    {
      printStatement();
    }
    else if (match(TOKEN_IF))
    {
        ifStatement();
    }
    else if (match(TOKEN_RETURN))
    {
        returnStatment();
    }
    else if (match(TOKEN_WHILE))
    {
        whileStatement();
    }
    else if (match(TOKEN_FOR))
    {
        forStatement();
    }
    else if (match(TOKEN_SWITCH))
    {
        // TODO fix switchStatement
        //switchStatement();
    }
    else if (match(TOKEN_LEFT_BRACE))
    {
        beginScope();
        block();
        endScope();
    }
    else
    {
      expressionStatement();
    }
  }

  int emitJump(uint8_t instruction)
  {
      emitByte(instruction);
      emitByte(0xff);
      emitByte(0xff);
      return currentChunk()->count - 2;
  }

  void emitLoop(int loopStart)
  {
      emitByte(OP_LOOP);

      int offset = currentChunk()->count - loopStart + 2;
      if (offset > UINT16_MAX) error("Loop body too large.");

      emitByte((offset >> 8) & 0xff);
      emitByte(offset & 0xff);
  }

  void patchJump(int offset)
  {
      int jump = currentChunk()->count - offset - 2;

      if (jump > UINT16_MAX)
      {
          error("To much code to jump over.");
      }

      currentChunk()->code[offset] = (jump >> 8) & 0xff;
      currentChunk()->code[offset + 1] = jump & 0xff;
  }

  void ifStatement()
  {
      consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
      expression();
      consume(TOKEN_RIGHT_PAREN, "Eexpect ')' after condition.");

      int thenJump = emitJump(OP_JUMP_IF_FALSE);
      emitByte(OP_POP);
      statement();

      int elseJump = emitJump(OP_JUMP);

      patchJump(thenJump);
      emitByte(OP_POP);

      if (match(TOKEN_ELSE))
      {
          statement();
      }
      patchJump(elseJump);
      
  }

  void returnStatment()
  {
      if (current->type == TYPE_SCRIPT)
      {
          error("Can`t return from top-level code.");
      }

      if (match(TOKEN_SEMICOLON))
      {
          emitReturn();
      }
      else
      {
          if (current->type == TYPE_INITIALIZER)
          {
              error("Can`t return a value from an initializer.");
          }
          expression();
          consume(TOKEN_SEMICOLON, "Expect ';' after return value.");
          emitByte(OP_RETURN);
      }
  }

  void whileStatement()
  {
      int loopStart = currentChunk()->count;
      consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
      expression();
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition. ");

      int exitJump = emitJump(OP_JUMP_IF_FALSE);

      emitByte(OP_POP);
      statement();

      emitLoop(loopStart);

      patchJump(exitJump);
      emitByte(OP_POP);
  }

  void forStatement()
  {
      beginScope();

      consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
      if (match(TOKEN_SEMICOLON))
      {
          // no initializer 
      }
      else if (match(TOKEN_VAR))
      {
          varDeclaration();
      }
      else
      {
          expressionStatement();
      }

      int loopStart = currentChunk()->count;
      int exitJump = -1;
      if (!match(TOKEN_SEMICOLON))
      {
          expression();
          consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

          exitJump = emitJump(OP_JUMP_IF_FALSE);
          emitByte(OP_POP);
      }
      if (!match(TOKEN_RIGHT_PAREN))
      {
          int bodyJump = emitJump(OP_JUMP);
          int incrementStart = currentChunk()->count;

          expression();
          emitByte(OP_POP);
          consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

          emitLoop(loopStart);
          loopStart = incrementStart;
          patchJump(bodyJump);
      }

      statement();

      emitLoop(loopStart);

      if (exitJump != -1)
      {
          patchJump(exitJump);
          emitByte(OP_POP);
      }

      endScope();
  }

  void switchStatement()
  {
      consume(TOKEN_LEFT_PAREN, "Expect '(' after 'switch'.");
      expression();
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition. ");
      consume(TOKEN_LEFT_BRACE, "Expect '{' after condition. ");
      
      emitByte(OP_TRUE);
      int loopStart = currentChunk()->count;
      int exitJump = emitJump(OP_JUMP_IF_FALSE);
      emitByte(OP_POP);
      while (match(TOKEN_CASE))
      {
          expression();
          consume(TOKEN_COLON, "Expect ':' after case expresion.");
          emitByte(OP_EQUAL);
          int thenJump = emitJump(OP_JUMP_IF_FALSE);
          emitByte(OP_POP);
          statement();
          emitByte(OP_FALSE);
          emitLoop(loopStart);

          patchJump(thenJump);
          emitByte(OP_POP); 
      }
      patchJump(exitJump);
      emitByte(OP_POP);

      consume(TOKEN_RIGHT_BRACE, "Expect '{' after condition. ");
  }


  void beginScope()
  {
      current->scopeDepth++;
  }

  void endScope()
  {
      current->scopeDepth--;

      while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth)
      {
        if (current->locals[current->localCount - 1].isCaptured)
        {
            emitByte(OP_CLOSE_UPVALUE);
        }
        else
        {
            emitByte(OP_POP);
        }

        current->localCount--;
      }

  }

  void block()
  {
      while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF))
      {
          declaration();
      }

      consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
  }

  void function(FunctionType type)
  {
      Compiler compiler;
      initCompiler(&compiler, type);
      beginScope();

      consume(TOKEN_LEFT_PAREN, "Expect '(' after function name.");
      if (!check(TOKEN_RIGHT_PAREN))
      {
          do {
              current->function->arity++;
              if (current->function->arity > 255)
              {
                  errorAtCurrent("Can`t have mor than 255 parametrs.");
              }
              uint8_t paramConstant = parseVariable("Expect parameter name.");
              defineVariable(paramConstant);
          } while (match(TOKEN_COMMA));
      }
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");

      consume(TOKEN_LEFT_BRACE, "Expect '{' before function body.");
      block();

      ObjFunction* function = endCompiler();
      emitBytes(OP_CLOSURE, makeConstant(OBJ_VAL(function)));

      for (int i = 0; i < function->upvalueCount; i++)
      {
          emitByte(compiler.upvalues[i].isLocal ? 1 : 0);
          emitByte(compiler.upvalues[i].index);
      }
  }

  void expressionStatement()
  {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after expression");
    emitByte(OP_POP);
  }

  void printStatement()
  {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';'' after value.");
    emitByte(OP_PRINT);
  }

  void number(bool) 
  {
    double  value = strtod(parser.previous.start, nullptr);
    emitConstant(NUMBER_VAL(value));
  } 

  void string(bool)
  {
      emitConstant(OBJ_VAL(vm::copyString(parser.previous.start + 1, parser.previous.length - 2)));
  }

  void variable(bool canAsigment)
  {
    namedVariable(parser.previous, canAsigment);
  }

  int resolveLocal(Compiler* compiler, Token* name)
  {
    for (int i = compiler->localCount - 1; i >= 0; i--)
    {
      auto local = &compiler->locals[i];
      if (identifiersEqual(name, &local->name))
      {
        if (local->depth == -1)
        {
          error("Can`t read local variable in its own initializer.");
        }
        return i;
      }
    }
    return -1;
  }
  int addUpvalue(Compiler* compiler, uint8_t index, bool isLocal)
  {
      int upvalueCount = compiler->function->upvalueCount;

      for (int i = 0; i < upvalueCount; i++)
      {
          Upvalue* upvalue = &compiler->upvalues[i];
          if (upvalue->index == index && upvalue->isLocal == isLocal)
          {
              return i;
          }
      }

      if (upvalueCount == UINT8_COUNT)
      {
          error("To many closure varibles in function");
          return 0;
      }

      compiler->upvalues[upvalueCount].isLocal = isLocal;
      compiler->upvalues[upvalueCount].index = index;
      return compiler->function->upvalueCount++;
  }
  int resolveUpvalue(Compiler* compiler, Token* name)
  {
      if (compiler->enclosing == nullptr)
      {
          return -1;
      }

      int local = resolveLocal(compiler->enclosing, name);
      if (local != -1)
      {
          compiler->enclosing->locals[local].isCaptured = true;
          return addUpvalue(compiler, (uint8_t)local, true);
      }

      int upvalue = resolveUpvalue(compiler->enclosing, name);
      if (upvalue != -1)
      {
          return addUpvalue(compiler, (uint8_t)upvalue, false);
      }
      return -1;
   }

  void namedVariable(Token name, bool canAsigment)
  {
      uint8_t getOp, setOp;
      int arg  = resolveLocal(current, &name);
      if  (arg != -1)
      {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;        
      }
      else if ((arg = resolveUpvalue(current, &name)) != -1)
      {
          getOp = OP_GET_UPVALUE;
          setOp = OP_SET_UPVALUE;   
      }
      else
      {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
      }
      if (canAsigment && match(TOKEN_EQUAL))
      {
          expression();
          emitBytes(setOp, arg);
      }
      else
      {
          emitBytes(getOp, arg);
      }
  }

  void this_(bool canAssign)
  {
      if (currentClass == nullptr)
      {
          error("Can't use 'this' outside class.");
          return;
      }
      variable(false);
  }

  void super_(bool canAssign)
  {
      if (currentClass == nullptr)
      {
          error("Can`t use 'super' outside of a class.");
      } 
      else if (!currentClass->hasSuperclass)
      {
          error("Can`t use 'super' in a class whit no superclass.");
      }
      consume(TOKEN_DOT, "Expect '.' after 'super'.");
      consume(TOKEN_IDENTIFIER, "Expect superclass method name.");
      uint8_t name = identifierConstant(&parser.previous);
      namedVariable(syntheticToken("this"), false);
      if (match(TOKEN_LEFT_PAREN))
      {
          uint8_t argCount = argumentList();
          namedVariable(syntheticToken("super"), false);
          emitBytes(OP_SUPER_INVOKE, name);
          emitByte(argCount);
      } 
      else
      {
          namedVariable(syntheticToken("super"), false);
          emitBytes(OP_GET_SUPER, name);
      }
  }

  void literal(bool)
  {
    switch (parser.previous.type) {
      case TOKEN_FALSE: emitByte(OP_FALSE); break;
      case TOKEN_NIL: emitByte(OP_NIL); break;  
      case TOKEN_TRUE: emitByte(OP_TRUE); break;
      default:
        return; // Unreachable.
    }                                             
  }  

  void dot(bool canAssign)
  {
      consume(TOKEN_IDENTIFIER, "Expect property name after '.'.");
      uint8_t name = identifierConstant(&parser.previous);

      if (canAssign && match(TOKEN_EQUAL))
      {
          expression();
          emitBytes(OP_SET_PROPERTY, name);
      }
      else if (match(TOKEN_LEFT_PAREN))
      {
          uint8_t argCount = argumentList();
          emitBytes(OP_INVOKE, name);
          emitByte(argCount);
      }
      else
      {
          emitBytes(OP_GET_PROPERTY, name);
      }
  }


  void unary(bool)
  {
    TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType)
    {
      case TOKEN_BANG: emitByte(OP_NOT); break;    
      case TOKEN_MINUS: emitByte(OP_NEGATE); break;    
      default:
        return;
    }
  }

  void and_(bool)
  {
      int endJump = emitJump(OP_JUMP_IF_FALSE);

      emitByte(OP_POP);
      parsePrecedence(PREC_AND);

      patchJump(endJump);
  }

  void or_(bool)
  {
      int elseJump = emitJump(OP_JUMP_IF_FALSE);
      int endJump = emitJump(OP_JUMP);

      patchJump(elseJump);
      emitByte(OP_POP);

      parsePrecedence(PREC_OR);
      patchJump(endJump);
  }

  void binary(bool)
  {
    TokenType operatorType = parser.previous.type;

    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) 
    {
      case TOKEN_BANG_EQUAL:    emitBytes(OP_EQUAL, OP_NOT); break;
      case TOKEN_EQUAL_EQUAL:   emitByte(OP_EQUAL); break;         
      case TOKEN_GREATER:       emitByte(OP_GREATER); break;   
      case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
      case TOKEN_LESS:          emitByte(OP_LESS); break;
      case TOKEN_LESS_EQUAL:    emitBytes(OP_GREATER, OP_NOT); break;
      case TOKEN_PLUS:          emitByte(OP_ADD); break;     
      case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
      case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
      case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;  
      default:
        return; // Unreachable.
    }
  }

  void grouping(bool)
  {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Experct ')' after expression. ");
  }

  uint8_t argumentList()
  {
      uint8_t argCount = 0;
      if (!check(TOKEN_RIGHT_PAREN))
      {
          do
          {
              expression();
              if (argCount == 255)
              {
                  error("Can`t have mor than 255 arguments.");
              }
              argCount++;
          } while(match(TOKEN_COMMA));
      }
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
      return argCount;
  }

  void call(bool canAssign)
  {
      uint8_t argCount = argumentList();
      emitBytes(OP_CALL, argCount);
  }
}

ObjFunction* compile(const char* source)
{
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler, TYPE_SCRIPT);    

    parser.hadError = false; 
    parser.panicMode = false;

    advance();
    while(!match(TOKEN_EOF))
    {
      declaration();
    }
    ObjFunction* function = endCompiler();
    //int line = -1;                                                              
    //for (;;) {                                                                  
    //  Token token = scanToken();                                                
    //  if (token.line != line) {                                                 
    //    printf("%4d ", token.line);                                             
    //    line = token.line;                                                      
    //  } else {                                                                  
    //    printf("   | ");                                                        
    //  }                                                                         
    //  printf("%2d '%.*s'\n", token.type, token.length, token.start); 
    //  if (token.type == TOKEN_EOF) break;      
    return parser.hadError ? nullptr : function;
  }

    void markCompilerRoots()
    {
        Compiler* compiler = current;
        while (compiler != nullptr)
        {
            memory::markObject((Obj*)compiler->function);
            compiler = compiler->enclosing;
        }
    }
}
}
