#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <windows.h>

int  main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Clock clock;
	float lastTime = 0;
	
	sf::Font font;
	if (!font.loadFromFile("../arial.ttf"))
	{
		//throw;
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		

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

		window.clear();
		
		
		window.draw(shape);
		window.draw(text);
		window.display();
	}

	return 0;
}