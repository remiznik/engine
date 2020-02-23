#include "rectangle.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>


namespace engine
{

	Rectangle::Rectangle()
	{
		sf::Vector2f size;
		size.x = 100;
		size.y = 100;
		shape_.setFillColor(sf::Color(0, 0, 0));
		shape_.setOutlineColor(sf::Color::Red);
		shape_.setOutlineThickness(2.);
		shape_.setSize(size);		
	}

	void Rectangle::setPosition(float x, float y)
	{
		shape_.setPosition(x, y);
	}

	void Rectangle::setSize(int w, int h)
	{
		shape_.setSize(sf::Vector2f((float)w, (float)h));
	}

	void Rectangle::draw(sf::RenderWindow& target)
	{
		target.draw(shape_);
	}
}