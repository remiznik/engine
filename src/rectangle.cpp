#include "rectangle.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>


namespace engine
{

	Rectangle::Rectangle()
	{
		sf::Vector2f size;
		size.x = 200;
		size.y = 344;
		shape_.setFillColor(sf::Color::Green);
		shape_.setSize(size);
	}

	void Rectangle::setPosition(int x, int y)
	{

	}

	void Rectangle::draw(sf::RenderWindow& target)
	{
		target.draw(shape_);
	}
}