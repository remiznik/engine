#include "vm/compiler.h"
#include "vm/scanner.h"

#include "vm/chunk.h"
#include "vm/debug.h"

#include "vm/common.h"
#include "vm/value.h"

#include "vm/object.h"

#include <stdio.h>
#include <cstdlib>

namespace script_system {
namespace vm {

namespace {

  void grouping();
  void unary();
  void binary();
  void number();
  void literal();
  void string();
  void variable();
  void printStatement();
  void statement();
  void expressionStatement();
  void varDeclaration();
  uint8_t parseVariable(const char* message);
  uint8_t identifierConstant(Token* name);
  void defineVariable(uint8_t global);
  void namedVariable(Token name);


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

  typedef void (*ParseFn)();

  typedef struct {        
    ParseFn prefix;       
    ParseFn infix;        
    Precedence precedence;
  } ParseRule;

  ParseRule rules[] = {                                              
    { grouping, NULL,    PREC_NONE },       // TOKEN_LEFT_PAREN      
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN     
    { NULL,     NULL,    PREC_NONE },      // TOKEN_LEFT_BRACE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE     
    { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_DOT             
    { unary,    binary,  PREC_TERM },       // TOKEN_MINUS           
    { NULL,     binary,  PREC_TERM },       // TOKEN_PLUS            
    { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON       
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_SLASH           
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_STAR            
    { unary,    NULL,    PREC_NONE },       // TOKEN_BANG            
    { NULL,     binary,  PREC_EQUALITY },   // TOKEN_BANG_EQUAL      
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL           
    { NULL,     binary,  PREC_EQUALITY },   // TOKEN_EQUAL_EQUAL     
    { NULL,     binary,  PREC_COMPARISON }, // TOKEN_GREATER         
    { NULL,     binary,  PREC_COMPARISON }, // TOKEN_GREATER_EQUAL   
    { NULL,     binary,  PREC_COMPARISON }, // TOKEN_LESS            
    { NULL,     binary,  PREC_COMPARISON }, // TOKEN_LESS_EQUAL      
    { variable, NULL,    PREC_NONE },       // TOKEN_IDENTIFIER      
    { string,   NULL,    PREC_NONE },       // TOKEN_STRING          
    { number,   NULL,    PREC_NONE },       // TOKEN_NUMBER          
    { NULL,     NULL,    PREC_NONE },       // TOKEN_AND             
    { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE            
    { literal,  NULL,    PREC_NONE },       // TOKEN_FALSE           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR             
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN             
    { NULL,     NULL,    PREC_NONE },       // TOKEN_IF              
    { literal,  NULL,    PREC_NONE },       // TOKEN_NIL             
    { NULL,     NULL,    PREC_NONE },       // TOKEN_OR              
    { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN          
    { NULL,     NULL,    PREC_NONE },       // TOKEN_SUPER           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_THIS            
    { literal,  NULL,    PREC_NONE },       // TOKEN_TRUE            
    { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR             
    { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR           
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF             
  };

  ParseRule* getRule(TokenType type) 
  {
    return &rules[type];                     
  }                                          


  Chunk* compilingChunk;
  Chunk* currentChunk()
  {
    return compilingChunk;
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

  void endCompiler()
  {
    emitReturn();
#ifdef DEBUG_PRINT_CODE                      
    if (!parser.hadError) 
    {                    
      disassembleChunk(currentChunk(), "code");
    }                                          
#endif                                       

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

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence)
    {
      advance();
      ParseFn infixRule = getRule(parser.previous.type)->infix;
      infixRule();
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
    return identifierConstant(&parser.previous);
  }

  uint8_t identifierConstant(Token* name)
  {
    return makeConstant(OBJ_VAL(copyString(name->start, name->length))); 
  }

  void defineVariable(uint8_t global)
  {
    emitBytes(OP_DEFINE_GLOBAL, global);
  }

  void statement()
  {
    if(match(TOKEN_PRINT))
    {
      printStatement();
    }
    else 
    {
      expressionStatement();
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

  void number() 
  {
    double  value = strtod(parser.previous.start, nullptr);
    emitConstant(NUMBER_VAL(value));
  } 

  void string()
  {
      emitConstant(OBJ_VAL(vm::copyString(parser.previous.start + 1, parser.previous.length - 2)));
  }

  void variable()
  {
    namedVariable(parser.previous);
  }

  void namedVariable(Token name)
  {
    uint8_t arg  = identifierConstant(&name);
    emitBytes(OP_GET_GLOBAL, arg);
  }

  void literal()
  {
    switch (parser.previous.type) {
      case TOKEN_FALSE: emitByte(OP_FALSE); break;
      case TOKEN_NIL: emitByte(OP_NIL); break;  
      case TOKEN_TRUE: emitByte(OP_TRUE); break;
      default:
        return; // Unreachable.
    }                                             
  }  



  void unary()
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

  void binary()
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

  void grouping()
  {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Experct ')' after expression. ");
  }

}

bool compile(const char* source, Chunk* chunk)
{
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false; 
    parser.panicMode = false;

    advance();
    while(!match(TOKEN_EOF))
    {
      declaration();
    }
    endCompiler();
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
    return !parser.hadError;
  }
}
}
