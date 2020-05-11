#include "script_system/script.h"
#include "graphics/scene.h"
#include "graphics/rectangle.h"

#include <assert.h>
#include "application.h"

#include "fps_lock.h"
#include <memory>

int  main()
{
	auto app = makeShared<app::Application>();
	if (!app->initialize())
		return -1;

	app->run();

	return 0;
}