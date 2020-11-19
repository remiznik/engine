#include "vm/compiler.h"
#include "vm/scanner.h"

#include "vm/chunk.h"
#include "vm/debug.h"

#include "vm/common.h"
#include "vm/value.h"

#include "vm/object.h"

#include "core/types.h"

#include <stdio.h>
#include <cstdlib>

namespace script_system {
namespace vm {

namespace {

  void grouping(bool);
  void unary(bool);
  void binary(bool);
  void number(bool);
  void literal(bool);
  void string(bool);
  void variable(bool);
  void and_(bool);
  void or_(bool);
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
  void whileStatement();
  void forStatement();
  void switchStatement();

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
  } Local;

  typedef enum {
      TYPE_FUNCTION,
      TYPE_SCRIPT
  } FunctionType;

  typedef struct {
      ObjFunction* function;
      FunctionType type;

      Local locals[UINT8_COUNT];
      int localCount;
      int scopeDepth;
  } Compiler;

  Compiler* current = nullptr;

  void initCompiler(Compiler* compiler, FunctionType type)
  {
    compiler->function = nullptr;
    compiler->type = type;

    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    compiler->function = newFunction();
    current = compiler;

    Local* local = &current->locals[current->localCount++];
    local->depth = 0;
    local->name.start = "";
    local->name.length = 0;
  }


  ParseRule rules[] = {                                              
    { grouping, nullptr,    PREC_NONE },       // TOKEN_LEFT_PAREN      
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_RIGHT_PAREN     
    { nullptr,     nullptr,    PREC_NONE },      // TOKEN_LEFT_BRACE
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_RIGHT_BRACE     
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_COMMA           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_DOT             
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
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_SUPER           
    { nullptr,     nullptr,    PREC_NONE },       // TOKEN_THIS            
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

  void declaration()
  {

    if (match(TOKEN_VAR))
    {
      varDeclaration();
    }
    else
    {
      statement();
    }


    if (parser.panicMode) synchronize();
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
  }

  void markInitialized()
  {
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
        emitByte(OP_POP);
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
  void namedVariable(Token name, bool canAsigment)
  {
      uint8_t getOp, setOp;
      int arg  = resolveLocal(current, &name);
      if  (arg != -1)
      {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;        
      }
      else
      {
        arg = identifierConstant(&name);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_LOCAL;
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

}

ObjFunction* compile(const char* source, Chunk* chunk)
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
}
}
