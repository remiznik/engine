namespace sf
{
	class RenderWindow;
}

namespace engine
{
	class Shape
	{
	public:
		virtual void draw(sf::RenderWindow& tartget_) = 0;
		virtual void setPosition(int x, int y) = 0;
	};
}