#include "shape.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace engine
{
	class Rectangle : public Shape
	{
	public:
		Rectangle();

		virtual void draw(sf::RenderWindow& target) override;
		virtual void setPosition(int x, int y) override;
	private:
		sf::RectangleShape shape_;		
	};
}