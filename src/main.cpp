#include <SFML/Graphics.hpp>


#include "scene.h"
#include "rectangle.h"
#include "fps_lock.h"

#include <memory>

int  main()
{

	engine::Scene scene;
	auto rectangle = std::make_shared<engine::Rectangle>();
	scene.addShape(rectangle);		
	
	sf::Clock clock;
	float lastTime = 0;
	utils::FpsLock lock(20);
	sf::Font font;
	if (!font.loadFromFile("../arial.ttf"))
	{
		//throw;
	}

	while (true)
	{

		if (!scene.draw())
			break;
		
		lock.update();
	}

	return 0;
}