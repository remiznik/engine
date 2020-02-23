#include "scene.h"
#include "shape.h"

#include <SFML/Window/Event.hpp>

namespace engine
{

	Scene::Scene() :
		renderTarget_(sf::VideoMode(500, 500), "SFML works!")
	{		
	}

	void Scene::addShape(const std::shared_ptr<Shape>& shape)
	{
		shapes_.push_back(shape);
	}

	bool Scene::draw()
	{
		if (renderTarget_.isOpen())
		{
			sf::Event event;
			while (renderTarget_.pollEvent(event))
			{
				// Close window: exit
				if (event.type == sf::Event::Closed)
				{
					renderTarget_.close();
					return false;
				}
			}

			renderTarget_.clear();
			for (auto shape : shapes_)
			{
				shape->draw(renderTarget_);
			}
			renderTarget_.display();
		}
		return true;
	}
}