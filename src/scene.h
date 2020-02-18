#include <SFML/Graphics/RenderWindow.hpp>

#include <vector>
#include <memory>

namespace engine
{
	class Shape;

	class Scene
	{
	public:
		Scene();

		void addShape(const std::shared_ptr<Shape>& shape);
		bool draw();
	private:
		std::vector<std::shared_ptr<Shape>> shapes_;
		sf::RenderWindow renderTarget_;
	
	};
}