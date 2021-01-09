#include "script_system/script.h"

int  main()
{
	script_system::Script script;
	script.init();
	

	auto result = script.run("../res/scripts/test.scr");

	script.fini();

	return 0;
}