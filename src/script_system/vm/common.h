#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "chunk.h"

#define DEBUG_PRINT_CODE
#define DEBUG_TRACE_EXECUTION

namespace script_system {
namespace vm {
	typedef enum {            
  		VAL_BOOL,               
  		VAL_NIL, 
  		VAL_NUMBER,             
	} ValueType; 

	typedef struct {  
  		ValueType type; 
  		union {         
    		bool boolean; 
    		double number;
  		} as; 
	} Value;	
	
#define IS_BOOL(value)    ((value).type == VAL_BOOL)  
#define IS_NIL(value)     ((value).type == VAL_NIL)   
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define AS_BOOL(value)    ((value).as.boolean)                       
#define AS_NUMBER(value)  ((value).as.number) 
#define BOOL_VAL(value)   ((Value){ VAL_BOOL, { .boolean = value } }) 
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })       
#define NUMBER_VAL(value) ((Value){ VAL_NUMBER, { .number = value } })
}
}