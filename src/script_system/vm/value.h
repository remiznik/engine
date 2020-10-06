#pragma once

#include "vm/common.h"

namespace script_system {
namespace vm {
//    typedef double Value;

    typedef enum {            
  		VAL_BOOL,               
  		VAL_NIL, 
  		VAL_NUMBER,             
	} ValueType; 

    struct Value
    {
        ValueType type;
        union {
            bool boolean;
            double number;
        } as;

        Value() : type(ValueType::VAL_NIL)
        {
            as.number = 0;
        }

        Value(bool boolean) : type(ValueType::VAL_BOOL)
        {
            as.boolean = boolean;
        }
        Value(double number) : type(ValueType::VAL_NUMBER)
        {
            as.number = number;
        }
    };
	
#define IS_BOOL(value)    ((value).type == VAL_BOOL)  
#define IS_NIL(value)     ((value).type == VAL_NIL)   
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define AS_BOOL(value)    ((value).as.boolean)                       
#define AS_NUMBER(value)  ((value).as.number) 
#define BOOL_VAL(value)   (Value(value)) 
#define NIL_VAL           (Value())       
#define NUMBER_VAL(value) (Value(value))

    typedef struct vm_value
    {
        int count;
        int capacity;
        script_system::vm::Value* values;
    } ValueArray;

    void initValueArray(ValueArray* array);              
    void writeValueArray(ValueArray* array, Value value);
    void freeValueArray(ValueArray* array); 

    bool valuesEqual(Value a, Value b);     

    void printValue(Value value);
}
}