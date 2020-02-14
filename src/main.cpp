#include <SFML/Graphics.hpp>
#include <windows.h>

#include "scene.h"
#include "rectangle.h"

#include <memory>

int  main()
{

	engine::Scene scene;
	auto rectangle = std::make_shared<engine::Rectangle>();
	scene.addShape(rectangle);
		
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Clock clock;
	float lastTime = 0;
	
	sf::Font font;
	if (!font.loadFromFile("../arial.ttf"))
	{
		//throw;
	}

	while (true)
	{
	

		char c[10];
		sprintf_s(c, "%f", lastTime);
		
		
		sf::Text text;
		text.setCharacterSize(24); 
		text.setString(sf::String(c));
		text.setFillColor(sf::Color::Red);
		text.setFont(font);
		

		float currentTime = clock.restart().asSeconds();
		float fps = 1.f / (currentTime - lastTime);
		lastTime = currentTime;

		
		scene.draw();
		
		
	}

	return 0;
}