#include "script_system/script.h"
#include "graphics/scene.h"
#include "graphics/rectangle.h"

#include "fps_lock.h"
#include <memory>
#include <iostream>

class Output : public core::Callable
{
public:

	core::Value call(const vector<core::Value>& args) override
	{
		std::cout << args[0].to<string>() << std::endl;
		return core::Value();
	}

	string toString() const  override
	{
		return "Output";
	}
};

int  main()
{
	script_system::Script script;
	script.registreFunction("output", makeShared<Output>());

	script.run("../res/scripts/test.scr");
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