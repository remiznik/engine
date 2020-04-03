#include "script_system/script.h"
#include "graphics/scene.h"
#include "graphics/rectangle.h"

#include "fps_lock.h"
#include <memory>

int  main()
{
	script_system::Script script;
	script.run();
	engine::Scene scene;

	auto rectangle = std::make_shared<engine::Rectangle>();
	scene.addShape(rectangle);		
	
	float lastTime = 0;
	utils::FpsLock lock(20);

	while (true)
	{

		if (!scene.draw())
			break;
		
		lock.update();
	}

	return 0;
}