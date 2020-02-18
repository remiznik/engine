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
		virtual void setPosition(float x, float y) = 0;
		virtual void setSize(int w, int h) = 0;
	};
}