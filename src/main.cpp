#include "script_system/script.h"

#include <assert.h>
#include "application.h"
int  main()
{
	auto app = makeShared<app::Application>();
	if (!app->initialize())
		return -1;
	
	app->run();

	return 0;
}