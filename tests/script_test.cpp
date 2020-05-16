#include "gtest/gtest.h"

#include "script.h"

class TestOutput : public core::Callable
{
public:
    TestOutput(string& o)
        : output_(o)
    {}

	core::Value call(const vector<core::Value>& args) override
	{
		output_ = args[0].to<string>();
		return core::Value();
	}

	string toString() const  override
	{
		return "TestOutput";
	}
private:
    string& output_;
};


TEST(Script, inheretence) 
{
    string result;
    script_system::Script script;
    script.registreFunction("output", makeShared<TestOutput>(result));
    
const char* text =
"class A {"
"    method() {"
"        print \"A method\";"
"    }"
"}"
"class B < A {"
"   method() {"
"       print \"B method\";"
"   }"
"   test() {"
"      super.method();"
"   }"
"}"
"class C < B {}"
"C().test();";

    script.execute(text);
    
    EXPECT_EQ (result, "A method");    
}
