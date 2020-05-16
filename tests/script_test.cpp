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

class ScriptTest : public ::testing::Test {
protected:
    void SetUp() override {  
        script.registreFunction("output", makeShared<TestOutput>(result));
    }
    // void TearDown() override {}

    script_system::Script script;
    string result;
};


TEST_F(ScriptTest, inheretence) 
{
   
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
