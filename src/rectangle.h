#include "shape.h"

#include <SFML/Graphics/RectangleShape.hpp>

namespace engine
{
	class Rectangle : public Shape
	{
	public:
		Rectangle();

		virtual void draw(sf::RenderWindow& target) override;
		virtual void setPosition(float x, float y) override;
		virtual void setSize(int w, int h) override;
	private:
		sf::RectangleShape shape_;		
	};
}