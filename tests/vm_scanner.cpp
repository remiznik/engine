
#include "gtest/gtest.h"
#include "script_system/vm/scanner.h"

TEST(get_letter, vm_scanner) 
{
    script_system::vm::initScanner("2");
    script_system::vm::Token token = script_system::vm::scanToken();

    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );
} 

TEST(simple_exr, vm_scanner) 
{
    script_system::vm::initScanner("2+3");
    script_system::vm::Token token;

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_PLUS );

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );
} 

    