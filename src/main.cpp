#include "script_system/script.h"

#include <assert.h>
#include "application.h"
int  main()
{
	script_system::Script script;
	script.run();
	engine::Scene scene;

	auto rectangle = std::make_shared<engine::Rectangle>();
	scene.addShape(rectangle);		
	
	auto app = makeShared<app::Application>();
	if (!app->initialize())
		return -1;

	auto result = script.run("../res/scripts/test.scr");
	app->run();

	script.fini();

	return 0;
}